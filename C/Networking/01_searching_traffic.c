#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include<pcap.h>
#include<stdio.h>
#include<string.h>

void flush()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

void doTCP(char *port, char *file_name) {
	pcap_t *pcap_file;
	char errbuf[PCAP_ERRBUF_SIZE];
	const u_char *packet;
	char line[50];
	int count = 0, i, j, riadky, counter = 0;
	struct pcap_pkthdr *packet_header = NULL;
	FILE *druha_vrstva, *tretia_vrstva, *stvrta_vrstva;

	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pcap_file == NULL)printf("Opening pcap file failed.\n");
	if ((druha_vrstva = fopen("2vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 2vrstva\n");
		return;
	}
	if ((tretia_vrstva = fopen("3vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 3vrstva\n");
		return;
	}
	if ((stvrta_vrstva = fopen("4vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 4vrstva\n");
		return;
	}

	int ethertype, IPprotocol, ports;
	while (fgets(line, 50, druha_vrstva) != NULL) {
		if (strstr(line, "ipvstyri") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &ethertype);
		}
	}
	while (fgets(line, 50, tretia_vrstva) != NULL) {
		if (strstr(line, "tcp") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &IPprotocol);
		}
	}
	while (fgets(line, 50, stvrta_vrstva) != NULL) {
		if (strstr(line, port) != NULL) {
			sscanf(line, "%*[^0123456789]%d", &ports);
		}
	}
	int pocet = 0;
	while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
		if ((packet[12] << 8 | packet[13]) == ethertype)
			if (packet[23] == IPprotocol)
				if ((packet[34] << 8 | packet[35]) == ports || (packet[36] << 8 | packet[37]) == ports)
					pocet++;
	}
	int pocet2 = 0;
	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pocet > 20)
		while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
			count++;
			if ((packet[12] << 8 | packet[13]) == ethertype)
				if (packet[23] == IPprotocol)
					if ((packet[34] << 8 | packet[35]) == ports || (packet[36] << 8 | packet[37]) == ports) {
						pocet2++;
						counter++;
						if (pocet2 <= 10 || pocet2 > (pocet - 10)) {
							printf("\nramec %d\n", count);
							printf("Dlzka ramca poskytnuta pcap API - %d\n", packet_header->caplen);
							printf("Dlzka ramca prenasaneho po mediu - %d\n", packet_header->len < 60 ? 64 : packet_header->len + 4);
							printf("Ethernet II\n");
							printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
								packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);
							printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
								packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
							printf("IPv4\n");
							printf("Zdrojova IP adresa: %d.%d.%d.%d\n", packet[26], packet[27], packet[28], packet[29]);
							printf("Cielova IP adresa: %d.%d.%d.%d\n", packet[30], packet[31], packet[32], packet[33]);
							printf("TCP\n");
							printf("zdrojovy port: %d\n", (packet[34] << 8 | packet[35]));
							printf("cielovy port: %d\n\n", (packet[36] << 8 | packet[37]));
							printf("Pocet ramcov daneho portu: %d\n\n", counter);
							riadky = packet_header->caplen / 16;
							for (i = 0; i <= riadky; i++) {
								for (j = 0; j < 16; j++) {
									if (j + (16 * i) < (*packet_header).caplen)
										printf("%02x ", packet[j + (16 * i)]);
								}
								printf("\n");
							}
						}
					}

		}
	else
		while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
			count++;
			if ((packet[12] << 8 | packet[13]) == ethertype) {
				if (packet[23] == IPprotocol)
					if ((packet[34] << 8 | packet[35]) == ports || (packet[36] << 8 | packet[37]) == ports) {
						counter++;
						printf("\nramec %d\n", count);
						printf("Dlzka ramca poskytnuta pcap API - %d\n", packet_header->caplen);
						printf("Dlzka ramca prenasaneho po mediu - %d\n", packet_header->len < 60 ? 64 : packet_header->len + 4);
						printf("Ethernet II\n");
						printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
							packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);
						printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
							packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
						printf("IPv4\n");
						printf("Zdrojova IP adresa: %d.%d.%d.%d\n", packet[26], packet[27], packet[28], packet[29]);
						printf("Cielova IP adresa: %d.%d.%d.%d\n", packet[30], packet[31], packet[32], packet[33]);
						printf("TCP\n");
						printf("zdrojovy port: %d\n", (packet[34] << 8 | packet[35]));
						printf("cielovy port: %d\n\n", (packet[36] << 8 | packet[37]));
						printf("Pocet ramcov daneho portu: %d\n\n", counter);
						riadky = packet_header->caplen / 16;
						for (i = 0; i <= riadky; i++) {
							for (j = 0; j < 16; j++) {
								if (j + (16 * i) < (*packet_header).caplen)
									printf("%02x ", packet[j + (16 * i)]);
							}
							printf("\n");
						}
					}
			}
		}


}

void doUDP(char *port, char *file_name) {
	pcap_t *pcap_file;
	char errbuf[PCAP_ERRBUF_SIZE];
	const u_char *packet;
	char line[50];
	int count = 0, i, j, riadky;
	struct pcap_pkthdr *packet_header = NULL;
	FILE *druha_vrstva, *tretia_vrstva, *stvrta_vrstva;

	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pcap_file == NULL)printf("Opening pcap file failed.\n");
	if ((druha_vrstva = fopen("2vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 2vrstva\n");
		return;
	}
	if ((tretia_vrstva = fopen("3vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 3vrstva\n");
		return;
	}
	if ((stvrta_vrstva = fopen("4vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 4vrstva\n");
		return;
	}

	int ethertype, IPprotocol, ports;
	while (fgets(line, 50, druha_vrstva) != NULL) {
		if (strstr(line, "ipvstyri") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &ethertype);
		}
	}
	while (fgets(line, 50, tretia_vrstva) != NULL) {
		if (strstr(line, "udp") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &IPprotocol);
		}
	}
	while (fgets(line, 50, stvrta_vrstva) != NULL) {
		if (strstr(line, port) != NULL) {
			sscanf(line, "%*[^0123456789]%d", &ports);
		}
	}

	while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
		count++;
		if ((packet[12] << 8 | packet[13]) == ethertype) {
			if (packet[23] == IPprotocol)
				if ((packet[34] << 8 | packet[35]) == ports || (packet[36] << 8 | packet[37]) == ports) {
					printf("\nramec %d\n", count);
					printf("Dlzka ramca poskytnuta pcap API - %d\n", packet_header->caplen);
					printf("Dlzka ramca prenasaneho po mediu - %d\n", packet_header->len < 60 ? 64 : packet_header->len + 4);
					printf("Ethernet II\n");
					printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
						packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);
					printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
						packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
					printf("IPv4\n");
					printf("Zdrojova IP adresa: %d.%d.%d.%d\n", packet[26], packet[27], packet[28], packet[29]);
					printf("Cielova IP adresa: %d.%d.%d.%d\n", packet[30], packet[31], packet[32], packet[33]);
					printf("UDP\n");
					printf("zdrojovy port: %d\n", (packet[34] << 8 | packet[35]));
					printf("cielovy port: %d\n\n", (packet[36] << 8 | packet[37]));
					riadky = packet_header->caplen / 16;
					for (i = 0; i <= riadky; i++) {
						for (j = 0; j < 16; j++) {
							if (j + (16 * i) < (*packet_header).caplen)
								printf("%02x ", packet[j + (16 * i)]);
						}
						printf("\n");
					}
				}
		}
	}
}

void doICMP(char *file_name) {
	pcap_t *pcap_file;
	char errbuf[PCAP_ERRBUF_SIZE];
	const u_char *packet;
	char line[50];
	int count = 0, i, j, riadky;
	struct pcap_pkthdr *packet_header = NULL;
	FILE *druha_vrstva, *tretia_vrstva, *stvrta_vrstva;

	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pcap_file == NULL)printf("Opening pcap file failed.\n");
	if ((druha_vrstva = fopen("2vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 2vrstva\n");
		return;
	}
	if ((tretia_vrstva = fopen("3vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 3vrstva\n");
		return;
	}

	int ethertype, IPprotocol;
	while (fgets(line, 50, druha_vrstva) != NULL) {
		if (strstr(line, "ipvstyri") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &ethertype);
		}
	}
	while (fgets(line, 50, tretia_vrstva) != NULL) {
		if (strstr(line, "icmp") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &IPprotocol);
		}
	}

	while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
		++count;
		if ((packet[12] << 8 | packet[13]) == ethertype) {
			if (packet[23] == IPprotocol) {
				printf("\nramec %d\n", count);
				printf("Dlzka ramca poskytnuta pcap API - %d\n", packet_header->caplen);
				printf("Dlzka ramca prenasaneho po mediu - %d\n", packet_header->len < 60 ? 64 : packet_header->len + 4);
				printf("Ethernet II\n");
				printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
					packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);
				printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
					packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
				printf("IPv4\n");
				printf("Zdrojova IP adresa: %d.%d.%d.%d\n", packet[26], packet[27], packet[28], packet[29]);
				printf("Cielova IP adresa: %d.%d.%d.%d\n", packet[30], packet[31], packet[32], packet[33]);
				printf("ICMP\n");
				printf("Type: %d\n\n", packet[34]);
				riadky = packet_header->caplen / 16;
				for (i = 0; i <= riadky; i++) {
					for (j = 0; j < 16; j++) {
						if (j + (16 * i) < (*packet_header).caplen)
							printf("%02x ", packet[j + (16 * i)]);
					}
					printf("\n");
				}
			}
		}
	}
}

void doARP(char *file_name) {
	pcap_t *pcap_file, *another_one;
	char errbuf[PCAP_ERRBUF_SIZE];
	const u_char *packet, *another_packet;
	char line[50];
	int count = 0;
	struct pcap_pkthdr *packet_header = NULL, *another_pckt_hdr = NULL;
	FILE *druha_vrstva;
	int riadky;
	int i, j;

	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pcap_file == NULL)printf("Opening pcap file failed.\n");
	another_one = pcap_open_offline(file_name, errbuf);
	if ((druha_vrstva = fopen("2vrstva.txt", "r")) == NULL) {
		printf("Nepodarilo sa otvorit 2vrstva\n");
		return;
	}
	int ethertype;
	while (fgets(line, 50, druha_vrstva) != NULL) {
		if (strstr(line, "arp") != NULL) {
			sscanf(line, "%*[^0123456789]%d", &ethertype);
		}
	}

	int pocet = 0, offset = 0;
	int cielovaIP1, zdrojovaIP1, cielovaIP2, zdrojovaIP2;
	while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
		count++;
		if ((packet[12] << 8 | packet[13]) == ethertype) {
			if (packet[21] == 1) {

				cielovaIP1 = (packet[38] << 24) + (packet[39] << 16) + (packet[40] << 8) + packet[41];
				zdrojovaIP1 = (packet[28] << 24) + (packet[29] << 16) + (packet[30] << 8) + packet[31];
				another_one = pcap_open_offline(file_name, errbuf);
				offset = 0;
				while (pcap_next_ex(another_one, &another_pckt_hdr, &another_packet) != -2) {
					offset++;
					if (offset > count)
						if ((another_packet[12] << 8 | another_packet[13]) == ethertype) {

							cielovaIP2 = (another_packet[38] << 24) + (another_packet[39] << 16) + (another_packet[40] << 8) + another_packet[41];
							zdrojovaIP2 = (another_packet[28] << 24) + (another_packet[29] << 16) + (another_packet[30] << 8) + another_packet[31];

							if (another_packet[21] == 2 && zdrojovaIP1 == cielovaIP2 && zdrojovaIP2 == cielovaIP1) {
								printf("\nKomunikacia c. %d\n", ++pocet);
								printf("ARP - Request, ");
								printf("IP adresa: %d.%d.%d.%d, MAC adresa: ???\n", packet[38], packet[39], packet[40], packet[41]);
								printf("Zdrojova IP: %d.%d.%d.%d, Cielova IP: %d.%d.%d.%d\n", packet[28], packet[29], packet[30], packet[31], packet[38], packet[39], packet[40], packet[41]);
								printf("ramec %d\n", count);
								printf("Dlzka ramca poskytnuta pcap API - %d\n", packet_header->caplen);
								printf("Dlzka ramca prenasaneho po mediu - %d\n", packet_header->len < 60 ? 64 : packet_header->len + 4);
								printf("Ethernet II\n");
								printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
									packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);
								printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n\n",
									packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);

								riadky = packet_header->caplen / 16;
								for (i = 0; i <= riadky; i++) {
									for (j = 0; j < 16; j++) {
										if (j + (16 * i) < (*packet_header).caplen)
											printf("%02x ", packet[j + (16 * i)]);
									}
									printf("\n");
								}

								printf("\nARP - Reply, ");
								printf("IP adresa: %d.%d.%d.%d, MAC adresa: ", another_packet[28], another_packet[29], another_packet[30], another_packet[31]);
								printf("%02d %02d %02d %02d\n", another_packet[32], another_packet[33], another_packet[34], another_packet[35]);
								printf("Zdrojova IP: %d.%d.%d.%d, Cielova IP: %d.%d.%d.%d\n", another_packet[28], another_packet[29], another_packet[30], another_packet[31], another_packet[38], another_packet[39], another_packet[40], another_packet[41]);
								printf("ramec %d\n", offset);
								printf("Dlzka ramca poskytnuta pcap API - %d\n", another_pckt_hdr->caplen);
								printf("Dlzka ramca prenasaneho po mediu - %d\n", another_pckt_hdr->len < 60 ? 64 : another_pckt_hdr->len + 4);
								printf("Ethernet II\n");
								printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
									another_packet[6], another_packet[7], another_packet[8], another_packet[9], another_packet[10], another_packet[11]);
								printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n\n",
									another_packet[0], another_packet[1], another_packet[2], another_packet[3], another_packet[4], another_packet[5]);
								riadky = another_pckt_hdr->caplen / 16;
								for (i = 0; i <= riadky; i++) {
									for (j = 0; j < 16; j++) {
										if (j + (16 * i) < (*another_pckt_hdr).caplen)
											printf("%02x ", another_packet[j + (16 * i)]);
									}
									printf("\n");
								}
							}
						}
				}
			}
		}

	}
}

void packet_IP_info(char *file_name) {
	const u_char *packet;
	struct pcap_pkthdr packet_header;
	unsigned int novy, ip;
	typedef struct ipecka {
		unsigned int ID;
		int data;
		struct ipecka *dalsi;
	} IPECKA;
	IPECKA *zac, *akt, *posledny = NULL;
	zac = NULL;

	pcap_t *pcap_file;
	char errbuf[PCAP_ERRBUF_SIZE];

	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pcap_file == NULL) {
		printf("Opening pcap file failed.\n");
		return;
	}

	printf("IP adresy vysielajucich uzlov: \n");
	while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
		akt = zac;
		if ((packet[12] << 8 | packet[13]) == 0x800) {
			novy = 1;
			ip = (packet[26] << 24) + (packet[27] << 16) + (packet[28] << 8) + packet[29];

			if (zac == NULL) {
				zac = (IPECKA *)malloc(sizeof(IPECKA));
				zac->dalsi = NULL;
				zac->data = (packet[16] << 8 | packet[17]) - 20;
				zac->ID = ip;
				akt = zac;
				printf("%d.%d.%d.%d\n", packet[26], packet[27], packet[28], packet[29]);
			}

			while (akt != NULL)
			{
				if (akt->ID == ip)
				{
					akt->data += (packet[16] << 8 | packet[17]) - 20;
					novy = 0;
					break;
				}
				posledny = akt;
				akt = akt->dalsi;
			}
			if (novy) {
				IPECKA *novy;
				novy = (IPECKA *)malloc(sizeof(IPECKA));
				posledny->dalsi = novy;
				novy->dalsi = NULL;
				novy->data = (packet[16] << 8 | packet[17]) - 20;
				novy->ID = ip;
				printf("%d.%d.%d.%d\n", packet[26], packet[27], packet[28], packet[29]);
			}
		}
	}

	akt = zac;
	IPECKA *max = akt;
	while (akt != NULL) {
		if (akt->data > max->data)
			max = akt;
		akt = akt->dalsi;
	}
	printf("\nAdresa uzla s najvacsim poctom odvysielanych bajtov:\n%d.%d.%d.%d  %d bajtov\n\n", max->ID >> 24, (max->ID << 8) >> 24, (max->ID << 16) >> 24, (max->ID << 24) >> 24);
}

void packet_info(char *file_name) {
	pcap_t *pcap_file;
	const u_char *packet;
	struct pcap_pkthdr *packet_header;
	char errbuf[PCAP_ERRBUF_SIZE];
	int count = 0;

	pcap_file = pcap_open_offline(file_name, errbuf);
	if (pcap_file == NULL) {
		printf("Opening pcap file failed.\n");
		return;
	}

	while (pcap_next_ex(pcap_file, &packet_header, &packet) != -2) {
		int riadky = packet_header->caplen / 16;
		int i, j;

		printf("Poradove cislo ramca: %d\n", ++count);
		printf("Dlzka ramca poskytnuta pcap API: %d\n", packet_header->caplen);
		printf("Dlzka ramca prenasaneho po mediu: %d\n", packet_header->len < 60 ? 64 : packet_header->len + 4);

		if ((packet[12] << 8 | packet[13]) > 0x5dc)
			printf("Typ ramca: Ethernet\n");
		else if (packet[14] == 0xaa)
			printf("Typ ramca: IEEE 802.3 - LLC - SNAP\n");
		else if (packet[14] == 0xff)
			printf("Typ ramca: IEEE 802.3 - Raw\n");
		else
			printf("Typ ramca: IEEE 802.3 - LLC\n");

		printf("Zdrojova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n",
			packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);
		printf("Cielova MAC adresa: %02x:%02x:%02x:%02x:%02x:%02x\n\n",
			packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);

		for (i = 0; i <= riadky; i++) {
			for (j = 0; j < 16; j++) {
				if (j + (16 * i) < packet_header->caplen)
					printf("%02x ", packet[j + (16 * i)]);
			}
			printf("\n");
		}
		printf("\n");
	}
	return;
}

void changeFile(char* file_name) {
	int i;
	char nieco[30];
	printf("\nNovy subor: ");
	flush();
	fgets(nieco, sizeof(nieco), stdin);
	for (i = 0; i < 30; i++)
		if (nieco[i] == '\n')nieco[i] = '\0';
	printf("\n%s je novy subor\n\n", nieco);
	strcpy(file_name, nieco);
}

int main() {
	pcap_t *pcap_file;
	char errbuf[PCAP_ERRBUF_SIZE];
	const u_char *packet;
	struct pcap_pkthdr packet_header;
	char file_name[30] = "trace-26.pcap";

	while (1) {
		int k;
		printf("Vyber:\n1 - Vsetky ramce + IP info\n2 - HTTP\n3 - HTTPS\n4 - TELNET\n5 - SSH\n6 - FTP riadiace\n7 - FTP datove\n8 - TFTP\n9 - ICMP\n10 - ARP\n11 - Zmen subor\n12 - special task\n13 - koniec\n");
		scanf_s("%d", &k);
		switch (k) {
		case 1: 			
			packet_info(file_name);
			packet_IP_info(file_name);
			break;
		case 2:	doTCP("http ", file_name);
			break;
		case 3:	doTCP("https", file_name);
			break;
		case 4:	doTCP("telnet", file_name);
			break;
		case 5:	doTCP("ssh", file_name);
			break;
		case 6:	doTCP("ftp-control", file_name);
			break;
		case 7:	doTCP("ftp-data", file_name);
			break;
		case 8:	doUDP("tftp", file_name);
			break;
		case 9:	doICMP(file_name);
			break;
		case 10:doARP(file_name);
			break;
		case 11: changeFile(file_name);
			break;
		case 12: doTCP("ftp-data", file_name);
			break;
		case 13: return;
			break;
		default: printf("Zla volba\n");
			break;
		}
	}


	getchar();
	getchar();
	return 0;
}