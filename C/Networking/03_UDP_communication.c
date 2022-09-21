#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ws2tcpip.h>
#include <string.h>
#include <time.h>
#define BUFFLEN 256

#pragma comment(lib, "Ws2_32.lib")

int receive_file(SOCKET communication, SOCKADDR_IN addr, int addr_len, int fragment_size);

DWORD WINAPI threadSendKeepAlive(SOCKET connection, SOCKADDR_IN addr);

struct sendto {
	SOCKET connection;
	SOCKADDR_IN addr;
};

//source: http://www.hackersdelight.org/hdcodetxt/crc.c.txt
unsigned reverse(unsigned x) {
	x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
	x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
	x = (x << 24) | ((x & 0xFF00) << 8) |
		((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

//source: http://www.hackersdelight.org/hdcodetxt/crc.c.txt
unsigned int crc32a(unsigned char *message, int size) {
	int i, j;
	unsigned int byte, crc;

	i = 0;
	crc = 0xFFFFFFFF;
	while (i < size) {
		byte = message[i];            // Get next byte.
		byte = reverse(byte);         // 32-bit reversal.
		for (j = 0; j <= 7; j++) {    // Do eight times.
			if ((int)(crc ^ byte) < 0)
				crc = (crc << 1) ^ 0x04C11DB7;
			else crc = crc << 1;
			byte = byte << 1;          // Ready next msg bit.
		}
		i = i + 1;
	}
	return reverse(~crc);
}

//client side functions
SOCKADDR_IN new_sockaddr() {
	SOCKADDR_IN addr;
	char ip[30];
	int port;

	printf("Zadaj IP: ");
	scanf("%s", ip);
	printf("\nZadaj port: ");
	scanf("%d", &port);

	inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	printf("IP: %s\nPort: %d\n", ip, port);

	return addr;
}

int establish_connection(SOCKET connection, SOCKADDR_IN addr, int addr_len, int fragment_size) {
	int inc_len, frag_size = 0;
	char establishing_message[10];

	establishing_message[0] = '0';
	establishing_message[1] = '1';
	establishing_message[2] = ((fragment_size << 1) >> 25);
	establishing_message[3] = ((fragment_size << 8) >> 24);
	establishing_message[4] = ((fragment_size << 16) >> 24);
	establishing_message[5] = ((fragment_size << 24) >> 24);

	memcpy(establishing_message + 2, &fragment_size, sizeof(fragment_size));

	//send establishing message
	if (sendto(connection, establishing_message, 10, 0, (SOCKADDR *)&addr, addr_len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	memset(establishing_message, '\0', 10);

	//recieve establishing message
	if ((inc_len = recv(connection, establishing_message, 10, 0)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}
	//check ci je sprava systemova a ci je na zacatie spojenia	
	if (!(establishing_message[0] == '0' && establishing_message[1] == '1'))
	{
		return 0;
	}
	else
	{
		memcpy(&frag_size, establishing_message + 2, sizeof(frag_size));
		//frag_size = (establishing_message[2] << 24) + (establishing_message[3] << 16) + (establishing_message[4] << 8) + establishing_message[5];
		if (!(frag_size == fragment_size))
			return 0;
	}

	//send establishing message
	if (sendto(connection, establishing_message, 10, 0, (SOCKADDR *)&addr, addr_len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;;
	}

	printf("\nConnected !\n");
	return 1;
}

int send_data(SOCKET connection, SOCKADDR_IN addr, char *data_to_send, int fragment_size, int fragment_number) {
	char *data, *buff;
	int data_header = 10;
	unsigned int checksum = crc32a(data_to_send, fragment_size);

	//if (strlen(data_to_send) > fragment_size) return 0;

	data = (char*)malloc(fragment_size + data_header);
	buff = (char*)malloc(data_header);

	data[0] = '1';
	data[1] = '1';
	memcpy(data + 2, &fragment_number, sizeof(fragment_number));
	memcpy(data + 6, &checksum, sizeof(checksum));
	memcpy(data + data_header, data_to_send, fragment_size);

	if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}
	while (1) {

		if ((recv(connection, buff, data_header, 0)) == SOCKET_ERROR)
		{
			printf("recv() failed with error code : %d", WSAGetLastError());
			getchar();
			return 0;
		}

		if (buff[0] == '1' && buff[1] == '0')
		{
			//znovu odoslat
			if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				getchar();
				return 0;
			}
		}
		else
			if (buff[0] == '1' && buff[1] == '1')
				return 1;
	}
	return 0;
}

int send_data_with_error_numero_duo(SOCKET connection, SOCKADDR_IN addr, int *fragment_number, int fragment_size) {
	char *data, *buff, *data_to_send;
	int data_header = 10, fragn = 0, k = 1;
	unsigned int checksum_chybny = crc32a("ahoj, checksum z tejto spravy bude iny ako checksum z dat ktore su v data_to_send", 20);
	unsigned int checksum;

	data_to_send = (char*)malloc(fragment_size);

	printf("Napis spravu, ktora sa posle s chybnym prvym a poslednym fragmentom: ");

	// prvy fragment
	fgets(data_to_send,fragment_size,stdin);
	fragn++;

	// ak sa sprava zmesti do jedneho fragmentu neposielaj dalsie
	if (strchr(data_to_send, '\n') != NULL)
		k = 0;

	printf("Posielam chybny fragment ...\n");

	checksum = checksum_chybny;
	
	data = (char*)malloc(fragment_size + data_header);
	buff = (char*)malloc(fragment_size + data_header);

	data[0] = '1';
	data[1] = '1';
	memcpy(data + 2, &fragn, sizeof(fragn));
	memcpy(data + 6, &checksum, sizeof(checksum));
	strcpy(data + data_header, data_to_send);

	if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	while (1) {
		if ((recv(connection, buff, data_header + fragment_size, 0)) == SOCKET_ERROR)
		{
			printf("recv() failed with error code : %d", WSAGetLastError());
			getchar();
			return 0;
		}

		if (buff[0] == '1' && buff[1] == '0')
		{
			//znovu odoslat s opravenou checksum hodnotou
			printf("Ziadost o preposlanie ... Preposielam ...\n");
			checksum = crc32a(data_to_send, fragment_size);
			memcpy(data + 6, &checksum, sizeof(checksum));

			if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				getchar();
				return 0;
			}
		}
		else
			if (buff[0] == '1' && buff[1] == '1')
				break;
	}

	while (k) {
		fgets(data_to_send, fragment_size, stdin) != NULL;
		fragn++;
		if (strchr(data_to_send, '\n') != NULL) {
			checksum = checksum_chybny;
			printf("Posielam chybny fragment ...\n");
			k = 0;
		}
		else {
			checksum = crc32a(data_to_send, fragment_size);
		}

		data[0] = '1';
		data[1] = '1';
		memcpy(data + 2, &fragn, sizeof(fragn));
		memcpy(data + 6, &checksum, sizeof(checksum));
		strcpy(data + data_header, data_to_send);

		if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			getchar();
			return 0;
		}
		while (1) {
			if ((recv(connection, buff, data_header + fragment_size, 0)) == SOCKET_ERROR)
			{
				printf("recv() failed with error code : %d", WSAGetLastError());
				getchar();
				return 0;
			}

			if (buff[0] == '1' && buff[1] == '0')
			{
				//znovu odoslat s opravenou checksum hodnotou
				printf("Ziadost o preposlanie ... Preposielam ...\n");
				checksum = crc32a(data_to_send, fragment_size);
				memcpy(data + 6, &checksum, sizeof(checksum));

				if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code : %d", WSAGetLastError());
					getchar();
					return 0;
				}
			}
			else
				if (buff[0] == '1' && buff[1] == '1') {
					printf("Uspesne potvrdenie !\n");
					break;
				}
		}
	}
	printf("foo\n");
	*fragment_number = fragn;

	return 1;
}

int send_data_with_error(SOCKET connection, SOCKADDR_IN addr, int fragment_number, int fragment_size) {
	char *data, *buff, *data_to_send;
	int data_header = 10;
	unsigned int checksum = crc32a("ahoj, checksum z tejto spravy bude iny ako checksum z dat ktore su v data_to_send", 20);

	//data ktore sa naozaj poslu su znaky 'a' alebo sprava
	data_to_send = (char*)malloc(fragment_size);
	if (fragment_size > 27) {
		strcpy(data_to_send, "Toto je preposlana sprava");
		data_to_send[25] = '\n';
		data_to_send[26] = '\0';
	}
	else {
		memset(data_to_send, 'a', fragment_size);
		data_to_send[fragment_size - 2] = '\n';
		data_to_send[fragment_size - 1] = '\0';
	}

	unsigned int checksum2 = crc32a(data_to_send, fragment_size);

	if (strlen(data_to_send) > fragment_size) {
		return 0;
	}

	data = (char*)malloc(fragment_size + data_header);
	buff = (char*)malloc(fragment_size + data_header);

	data[0] = '1';
	data[1] = '1';
	memcpy(data + 2, &fragment_number, sizeof(fragment_number));
	memcpy(data + 6, &checksum, sizeof(checksum));
	strcpy(data + data_header, data_to_send);

	printf("Posielam spravu \"%s\" s chybnou checksum hodnotou ...\n", data_to_send);
	if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}
	while (1) {
		if ((recv(connection, buff, data_header + fragment_size, 0)) == SOCKET_ERROR)
		{
			printf("recv() failed with error code : %d", WSAGetLastError());
			getchar();
			return 0;
		}

		if (buff[0] == '1' && buff[1] == '0')
		{
			//znovu odoslat s opravenou checksum hodnotou
			printf("Ziadost o preposlanie ... Preposielam ...\n");
			memcpy(data + 6, &checksum2, sizeof(checksum2));

			if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				getchar();
				return 0;
			}
		}
		else
			if (buff[0] == '1' && buff[1] == '1')
				return 1;
	}
	return 0;
}

int send_file(SOCKET connection, SOCKADDR_IN addr, int addr_len, int fragment_size, char *filename) {
	FILE *fd;
	struct _stat stats;
	char *buff;
	off_t offset;
	int remain_data, fragn = 0;
	long file_size;

	buff = (char*)malloc(fragment_size);

	filename[strlen(filename)-1] = '\0';

	fd = fopen(filename,"rb");
	if (!fd) {
		printf("Failed to open file %s\n", filename);
		return 1;
	}

	_stat(filename, &stats);
	file_size = stats.st_size;

	printf("File size: %ld\n", file_size);

	//send "!file" so that receiving side knows to initiate file transfer
	send_data(connection, addr, "!file", fragment_size,0);
	memset(buff, 0, fragment_size);
	//send file name
	strcpy(buff, filename);
	send_data(connection, addr, buff, fragment_size, 0);

	memset(buff, 0, fragment_size);
	//send file size
	sprintf(buff, "%ld", file_size);
	printf("Sending this file_size: %s\n", buff);
	send_data(connection, addr, buff, fragment_size, 0);

	offset = 0;
	memset(buff, 0, fragment_size);
	//posielanie suboru
	while (file_size > 0) {
		//vynulovanie buff
		memset(buff, 0, fragment_size);

		//nacitanie dat suboru
		fread(buff,sizeof(byte),fragment_size,fd);

		//poslanie dat
		if (!send_data(connection, addr, buff, fragment_size, fragn++)) {
			printf("Error sending file data\n");
			return 1;
		}

		//vypocitanie zostavajucich dat
		file_size -= fragment_size;
	}

	if (!send_end_of_data(connection, addr, --fragn, fragment_size))
		return 1;

	fclose(fd);

	return 0;
}

int send_end_of_data(SOCKET connection, SOCKADDR_IN addr, int fragment_number, int fragment_size) {
	char *data;
	int data_header = 10;
	unsigned int checksum = 0;
	int inc_len, tempFragNumber;

	data = (char*)malloc(fragment_size + data_header);

	memset(data, '\0', fragment_size + data_header);

	data[0] = '1';
	data[1] = '0';
	memcpy(data + 2, &fragment_number, sizeof(fragment_number));
	memcpy(data + 6, &checksum, sizeof(checksum));

	if (sendto(connection, data, fragment_size + data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	if ((inc_len = recv(connection, data, fragment_size + data_header, 0)) == SOCKET_ERROR)
	{
		printf("recv() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	memcpy(&tempFragNumber, data + 2, sizeof(tempFragNumber));

	if (!(data[0] == '1' && data[1] == '0' && fragment_number == tempFragNumber))
	{
		printf("Zle potvrdenie end_of_data ! %d not equal %d\n",fragment_number, tempFragNumber);
		getchar();
		return 0;
	}
	return 1;
}

int end_connection(SOCKET connection, SOCKADDR_IN addr, int addr_len, int fragment_size) {
	int inc_len, header = 10;
	char *ending_message;

	ending_message = (char*)malloc(fragment_size + header);

	memset(ending_message, '\0', fragment_size + header);

	ending_message[0] = '0';
	ending_message[1] = '0';

	//send ending message
	if (sendto(connection, ending_message, fragment_size + header, 0, (SOCKADDR *)&addr, addr_len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	memset(ending_message, '\0', fragment_size + header);

	//recieve ending message
	if ((inc_len = recv(connection, ending_message, fragment_size + header, 0)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}
	//check ci je sprava systemova a ci je na ukoncenie spojenia	
	if (!(ending_message[0] == '0' && ending_message[1] == '0'))
	{
		printf("Wrong reply to ending message!\n");
		return 0;
	}

	//send ending message
	if (sendto(connection, ending_message, fragment_size + header, 0, (SOCKADDR *)&addr, addr_len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;;
	}

	printf("\nConnection ended !\n");
	return 1;
}

int send_message(SOCKET connection, SOCKADDR_IN addr, int fragment_size) {
	//user input + fragmentacia + odosielanie + cakanie na potvrdenie + ak !exit tak koniec
	int fragment_number = 0;
	char *buff, filename[50] = {'\0'}, c, exit_control[7] = "!exit", fragSize_control[8] = "!fsize", error_control[8] = "!error", file_control[7] = "!file";
	fragSize_control[6] = '\n';
	fragSize_control[7] = '\0';
	error_control[6] = '\n';
	error_control[7] = '\0';
	exit_control[5] = '\n';
	exit_control[6] = '\0';
	file_control[5] = '\n';
	file_control[6] = '\0';
	buff = (char*)malloc(fragment_size);


	//while ((c = getchar()) != '\n' && c != EOF) {}

	printf("YOU: ");

	while (1) {
		fgets(buff, fragment_size, stdin);

		if (strcmp(buff, exit_control) == 0) {
			end_connection(connection, addr, sizeof(addr), fragment_size);
			return 0;
		}

		if (strcmp(buff, file_control) == 0) {
			printf("Name of file to send: ");
			fgets(filename,50, stdin);
			if (send_file(connection, addr, sizeof(addr), fragment_size, filename)) 
				printf("Error sending file %s\n",filename);
			
			return 1;
		}

		if (strcmp(buff, fragSize_control) == 0) {
			printf("Fragment size: %d B\n", fragment_size);
			return 1;
		}

		if (strcmp(buff, error_control) == 0) {
			if (!send_data_with_error_numero_duo(connection, addr, &fragment_number, fragment_size))
			{
				printf("Chyba pri odosielani!\n");
				end_connection(connection, addr, sizeof(addr), fragment_size);
				return 0;
			}
			printf("Posielam end of data\n");
			if (send_end_of_data(connection, addr, fragment_number, fragment_size))
				return 1;
			break;
		}

		if (!send_data(connection, addr, buff, fragment_size, ++fragment_number))
		{
			printf("Chyba pri odosielani!\n");
			end_connection(connection, addr, sizeof(addr), fragment_size);
			return 0;
		}

		//po urc fragmentoch cakat na potvrdenie


		if (strchr(buff, '\n') != NULL) break;
	}
	//ukoncenie posielania + potvrdenie
	if (send_end_of_data(connection, addr, fragment_number, fragment_size))
		return 1;
	end_connection(connection, addr, sizeof(addr), fragment_size);
	return 0;
}

void make_connection() {
	SOCKADDR_IN addr;
	SOCKET connection = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int fragment_size, addr_len = sizeof(addr);

	addr = new_sockaddr();

	printf("Velkost fragmentu v bajtoch: ");
	scanf("%d", &fragment_size);

	char c = getchar();

	if (!establish_connection(connection, addr, addr_len, fragment_size))
		return;

	struct sendto *to = malloc(sizeof(struct sendto));
	to->addr = addr;
	to->connection = connection;

	//zacat posielat keepalive
	CreateThread(NULL, 0, threadSendKeepAlive, to, 0, NULL);

	printf("(commands do not work when fragment size is less than 8 B)\nPre ukoncenie napis !exit\nPre velkost fragmentu napis !fsize\nPre odoslanie chybneho ramca napis !error\nPre poslanie suboru napis !file\n");
	while (send_message(connection, addr, fragment_size));

}

//server side functions
int establish_connection_receive(SOCKET communication, SOCKADDR_IN *new_addr, int *new_addr_len, int *fragment_size) {
	char buff[BUFFLEN] = { '\0' };
	int inc_len, frag_size = 0;

	//recieve establishing message
	if ((inc_len = recvfrom(communication, buff, BUFFLEN, 0, (SOCKADDR *)&(*new_addr), &(*new_addr_len))) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}
	//check ci je sprava systemova a ci je na zacatie spojenia	
	if (!(buff[0] == '0' && buff[1] == '1'))
	{
		*fragment_size = 0;
		return 0;
	}
	else
	{
		memcpy(&frag_size, buff + 2, sizeof(frag_size));
		//frag_size = (buff[2] << 24) + (buff[3] << 16) + (buff[4] << 8) + buff[5];
		if (frag_size > 0)
		{
			*fragment_size = frag_size;
		}
		else
		{
			//keep alive type
			return 0;
		}
	}

	//reply the same
	if (sendto(communication, buff, inc_len, 0, (SOCKADDR *)&(*new_addr), &(*new_addr_len)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	memset(buff, '\0', BUFFLEN);

	//recieve establishing message
	if ((inc_len = recvfrom(communication, buff, inc_len, 0, (SOCKADDR *)&(*new_addr), &(*new_addr_len))) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	//check ci je sprava systemova a ci je na zacatie spojenia	
	if (!(buff[0] == '0' && buff[1] == '1'))
	{
		*fragment_size = 0;
		return 0;
	}
	else
	{
		memcpy(&frag_size, buff + 2, sizeof(frag_size));
		//frag_size = (buff[2] << 24) + (buff[3] << 16) + (buff[4] << 8) + buff[5];
		if (frag_size > 0)
		{
			*fragment_size = frag_size;
		}
		else
		{
			//keep alive type
			return 0;
		}
	}

	printf("Connection established with %s\n", inet_ntoa((*new_addr).sin_addr));
	return 1;
}

int receive_data(SOCKET communication, SOCKADDR_IN new_addr, int new_addr_len, int fragment_size, char **data, int *fragment_number) {
	int inc_len, tempFragNumber;
	unsigned int checksum;
	int data_header = 10;
	char *buff = (char*)malloc(fragment_size + data_header);

	if ((inc_len = recvfrom(communication, buff, fragment_size + data_header, 0, (SOCKADDR *)&new_addr, &new_addr_len)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	memcpy(&tempFragNumber, buff + 2, sizeof(tempFragNumber));

	if (!(buff[0] == '1' && buff[1] == '1'))
	{

		if (buff[0] == '1' && buff[1] == '0' && tempFragNumber == *fragment_number)  ///////
			return (-1);
		if (buff[0] == '0' && buff[1] == '0')
			return 0;
		if (buff[0] == '0' && buff[1] == '1' && tempFragNumber == 0) {
			received_keep_alive(communication,new_addr);
			return 3;
		}
		printf("%c %c - %d not equal %d\n",buff[0],buff[1], tempFragNumber, *fragment_number);
		printf("Zla hlavicka pri prijimani!\n");
		return 0;
	}

	memcpy(&checksum, buff + 6, sizeof(checksum));
	memcpy(fragment_number, buff + 2, sizeof(*fragment_number));

	if (checksum != crc32a(buff + data_header, fragment_size))
	{
		//poziadat o preposlanie
		buff[1] = '0';
		printf("Zla checksum hodnota, ziadam o preposlanie...\n");
		if (sendto(communication, buff, data_header, 0, (SOCKADDR *)&new_addr, sizeof(new_addr)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			getchar();
			return 0;
		}
		return 3;
	}
	else
	{
		//potvrdit prijatie
		if (sendto(communication, buff, data_header, 0, (SOCKADDR *)&new_addr, sizeof(new_addr)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			getchar();
			return 0;
		}

		if (strcmp(buff + data_header, "!file") == 0) {
			return 4;
		}

	}

	//strcpy(*data, buff + data_header);
	memcpy(*data, (buff + data_header), fragment_size);

	return 1;
}

int send_end_of_data_receive(SOCKET communication, SOCKADDR_IN new_addr, int fragment_number) {
	char *data;
	int data_header = 10;
	int checksum = 25214;
	int inc_len;

	data = (char*)malloc(data_header);

	data[0] = '1';
	data[1] = '0';
	memcpy(data + 2, &fragment_number, sizeof(fragment_number));
	memcpy(data + 6, &checksum, sizeof(checksum));

	if (sendto(communication, data, data_header, 0, (SOCKADDR *)&new_addr, sizeof(new_addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}
	return 1;
}

int receiving(SOCKET communication, SOCKADDR_IN new_addr, int new_addr_len, int fragment_size) {
	int fragment_number = 0;
	char *buff = (char*)malloc(fragment_size);
	char *data = (char*)malloc(fragment_size);
	memset(data, '\0', fragment_size);
	int size_of_data = fragment_size;
	int receive_type;

	while (1)
	{

		if ((receive_type = receive_data(communication, new_addr, new_addr_len, fragment_size, &buff, &fragment_number)) <= 0)
		{
			if (receive_type < 0)
				if (send_end_of_data_receive(communication, new_addr, fragment_number))
					break;
			return 0;
		}

		if (receive_type == 3) continue;

		if (receive_type == 4) {
			receive_file(communication, new_addr, new_addr_len, fragment_size, &fragment_number);
			continue;
		}

		if ((fragment_size * fragment_number) > size_of_data)
		{
			size_of_data = fragment_number * fragment_size;
			data = realloc(data, size_of_data);
			memset(data + (fragment_size * (fragment_number - 1)) + 1, '\0', fragment_size - 1);
		}
		strcat(data, buff);
	}
	printf("THE OTHER ONE: %s", data);
	return 1;
}

int end_connection_receive(SOCKET connection, SOCKADDR_IN addr, int addr_len, int fragment_size) {
	int inc_len, header = 10;
	char *ending_message;

	ending_message = (char*)malloc(fragment_size + header);

	memset(ending_message, '\0', fragment_size + header);

	ending_message[0] = '0';
	ending_message[1] = '0';

	//send ending message
	if (sendto(connection, ending_message, fragment_size + header, 0, (SOCKADDR *)&addr, addr_len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;;
	}

	memset(ending_message, '\0', fragment_size + header);

	//recieve establishing message
	if ((inc_len = recv(connection, ending_message, fragment_size + header, 0)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		getchar();
		return 0;
	}

	//check ci je sprava systemova a ci je na ukoncenie spojenia	
	if (!(ending_message[0] == '0' && ending_message[1] == '0'))
	{
		printf("Wrong reply to ending message!\n");
		return 0;
	}

	printf("\nConnection ended !\n");
	return 1;
}

void start_receiving(SOCKET communication) {
	SOCKADDR_IN new_addr;
	int fragment_size, new_addr_len = sizeof(new_addr);


	if (!establish_connection_receive(communication, &new_addr, &new_addr_len, &fragment_size))
		return;

	printf("Fragment size: %d B\n\n", fragment_size);

	while (receiving(communication, new_addr, new_addr_len, fragment_size));

	end_connection_receive(communication, new_addr, new_addr_len, fragment_size);

	
}

int receive_file(SOCKET communication, SOCKADDR_IN addr, int addr_len, int fragment_size, int *fragment_number) {
	char *data = (char*)malloc(fragment_size);
	char *file_name = (char*)malloc(100);
	long file_size;
	FILE *received_file;
	int remain_data = 0;
	strcpy(file_name, "copy-");

	//prijatie nazvu suboru
	receive_data(communication, addr, addr_len, fragment_size, &data, fragment_number);
	strcat(file_name, data);

	//prijatie velkosti suboru
	receive_data(communication, addr, addr_len, fragment_size, &data, fragment_number);
	file_size = atol(data);

	received_file = fopen(file_name, "wb");
	if (received_file == NULL) {
		fprintf(stderr, "Failed to open file --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	remain_data = file_size;
	
	printf("\nReceiving file...\n");
	if (file_size > 1000000)
		printf("Ocakavy cas: %d s\n", file_size / 1000000);

	//prijatie dat a ich zapisovanie do suboru
	while (remain_data > 0) {
		receive_data(communication, addr, addr_len, fragment_size, &data, fragment_number);
		
		fwrite(data, sizeof(byte), remain_data > fragment_size ? fragment_size : remain_data, received_file);

		remain_data -= fragment_size;
	}

	printf("\nReceived file: %s (%ld B)\n\n", file_name, file_size);

	fclose(received_file);

	return 0;
}

int send_keep_alive(SOCKET connection, SOCKADDR_IN addr) {
	char *data, *buff;
	int data_header = 10, fragment_number = 0;

	buff = (char*)malloc(data_header);
	data = (char*)malloc(data_header);

	data[0] = '0';
	data[1] = '1';
	memcpy(data + 2, &fragment_number, sizeof(fragment_number));

	if (sendto(connection, data, data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 1;
	}
	while (1) {

		if ((recv(connection, buff, data_header, 0)) == SOCKET_ERROR)
		{
			printf("recv() failed with error code : %d", WSAGetLastError());
			getchar();
			return 1;
		}

		memcpy(&fragment_number, buff + 2, sizeof(fragment_number));

		if (buff[0] == '0' && buff[1] == '1' && fragment_number == 0)
			return 0;
		else {
			printf("Zle potvrdenie keep alive!\n");
			getchar();
			return 1;
		}
	}
	return 0;
}

int received_keep_alive(SOCKET connection, SOCKADDR_IN addr) {
	char *data, *buff;
	int data_header = 10, fragment_number = 0;

	data = (char*)malloc(data_header);
	buff = (char*)malloc(data_header);

	printf("Received keep alive\n");

	data[0] = '0';
	data[1] = '1';
	memcpy(data + 2, &fragment_number, sizeof(fragment_number));

	if (sendto(connection, data, data_header, 0, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		getchar();
		return 1;
	}
	return 0;
}

DWORD WINAPI threadSendKeepAlive(void *data) {


	struct sendto *to = (struct sendto *)data;
	SOCKET connection = to->connection;
	SOCKADDR_IN addr = to->addr;

	while (1) {

		Sleep(80000);
		if (send_keep_alive(connection, addr)) {
			printf("Error sending keep alive !\n");
			getchar();
			return 1;
		}
	}

	return 0;
}

int main() {
	int port, k = 1, s;
	SOCKADDR_IN addr;
	SOCKET communication;
	WSADATA wsa;

	//inicializacia
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsa) != 0) {
		MessageBoxA(NULL, "Startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	//vytvorenie socketu
	if ((communication = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		return 1;
	}

	printf("Zadaj port pre prichadzajuce spojenie: ");
	scanf("%d", &port);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);


	//bind socketu
	if (bind(communication, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		return 1;
	}

	while (k) {
		printf("1 - Client\n2 - Server\n3 - Neither\n\nChoose side: ");

		scanf("%d", &s);
		getchar();
		switch (s) {
		case 1:
			make_connection();
			break;
		case 2:
			start_receiving(communication);
			break;
		case 3: k = 0;
			break;
		default:
			printf("\nYou chose wrong.\n");
		}
	}

	return 0;
}