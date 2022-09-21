#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "functions.h"

#define TRUE 1
#define BUFF 100
#define PORT_NUMBER     5003		//cislo portu
//#define SERVER_ADDRESS  "127.0.0.1"	//pre testovacie uceli
#define FILENAME        "temp"		//nazov docasneho suboru

//funckia pre server spravanie
void server(){
	int server_socket;
        int peer_socket;
        socklen_t       sock_len;
        ssize_t len;
        struct sockaddr_in      server_addr;
        struct sockaddr_in      peer_addr;
	int i, n, koniec = 1;
	int opt = TRUE;  
    	int new_socket, client_socket[30], max_clients = 30, activity, sd;  //premenne pre viacere sucasne spojenia
    	int max_sd;  
	char buff[100];			// buffer pre prijatu spravu

	printf("Running as server ...\n");

	//mnozina socketov
    	fd_set readfds; 
	
	//inicializacia socketov na nepouzivane 
	for (i = 0; i < max_clients; i++) 
		client_socket[i] = 0;
	
	//vytvorenie server socketu
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1){
                fprintf(stderr, "Error creating socket --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
	
	//nastavenie fungovania pre viacere spojenia
	if( setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){  
        	perror("setsockopt");  
        	exit(EXIT_FAILURE);  
    	} 

	//vynulovanie server_addr
        memset(&server_addr, 0, sizeof(server_addr));
	
        //naplnenie server_addr
        server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT_NUMBER);
	
	//bind na socket
        if ((bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) == -1){
                fprintf(stderr, "Error on bind --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
	
	//pocuvanie pre server_socket
        if ((listen(server_socket, 5)) == -1){
                fprintf(stderr, "Error on listen --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }

	//dlzka sockaddr
	sock_len = sizeof(struct sockaddr_in);

	//hlavny cyklus cakania na clientov a ich obsluhovanie
	while(koniec){  
	
		//vycistenie mnoziny socketov 
		FD_ZERO(&readfds);  
	    
		//pridanie server socketu
		FD_SET(server_socket, &readfds);  
		max_sd = server_socket;  
		    
		//pridanie child socketov 
		for ( i = 0 ; i < max_clients ; i++){  
			//socket 
		    	sd = client_socket[i];  
		        
			//ak je pouzivany tak pridaj do mnoziny na citanie socketov 
			if(sd > 0)  
				FD_SET( sd , &readfds);  
				
			//zistenie najvacsieho socketu pre funkciu select
			if(sd > max_sd)  
				max_sd = sd;  
		}  
	    
		//cakanie na aktivitu na nejakom sockete
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
	      
		if ((activity < 0) && (errno!=EINTR)){  
			printf("select error");  
		}  
		    
		//ak je aktivita na master sockete tak je to nove spojenie 
		if (FD_ISSET(server_socket, &readfds)){  
			if ((new_socket = accept(server_socket, 
		            	(struct sockaddr *)&peer_addr, (socklen_t*)&sock_len))<0){  
		      		perror("accept");  
		        	exit(EXIT_FAILURE);  
		    	}  
		    
		    	//informovanie o novom spojeni 
		    	printf("New connection - %d\tip: %s\tport: %d \n", new_socket, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));  
		        
		    	//pridanie noveho socketu
		    	for (i = 0; i < max_clients; i++) 
		        	if( client_socket[i] == 0 ){  
		            		client_socket[i] = new_socket;                        
		            		break;  
		        	}  
		}  
		    
		//ak to nie je nove spojenie tak je to aktivita na nejakom aktivnom sockete
		for (i = 0; i < max_clients; i++){  
		   	sd = client_socket[i];  
		    	
			//ak je aktivita na tomto sockete
		    	if (FD_ISSET( sd , &readfds)){
		        	n = read(sd, buff, sizeof(buff));	//prijatie prikazu
		    		buff[n] = '\0';				//zbavenie sa \n
				printf("PRIKAZ: %s\n", buff);		//vypisanie prikazu
				if ((n = execute_command(buff)) > -2) {	//vykonanie prikazu
		        		if (n == 2) {			//ak je prikaz make
						write(sd, buff, sizeof(buff));
		            			recieve_file(sd);
		        		}
		        		else if (n == 0)		//ak je prikaz halt
		            			koniec = 0;
			
		    		}
				else {						//prikaz je quit
					close( sd );  
		            		client_socket[i] = 0;
					printf("Disconnected - %d\n", sd);
				}
		
				memset(buff, 0, sizeof(buff));			//vynulovanie bufferu
			} 
		}
	}

	close(peer_socket);	//zatvorenie socketu
	close(server_socket);
}

//funkcia pre client spravanie
void client(){
	int client_socket, length, koniec = 1;
        ssize_t len;
	size_t len_prikaz = BUFF;
        struct sockaddr_in remote_addr;
	FILE *f;
	char file[50];
	char ip_addr[20];
	char *prikaz;
	prikaz = malloc(BUFF * sizeof(char));
	memset(prikaz, 0, BUFF);

	//vynulovanie addr
        memset(&remote_addr, 0, sizeof(remote_addr));
	//nastavenie addr
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(PORT_NUMBER);
	
	//nastavenie IP
	while(1){
		printf("Running as client ...\nIP to connect to: ");
		scanf("%s", ip_addr);
		if(inet_pton(AF_INET, ip_addr, &(remote_addr.sin_addr)) == 1)
			break;
			
		printf("Wrong IP format!\n");
	}	
		
	//vytvorenie socketu
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1){
                fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

        //pripojenie na server
        if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1){
                fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

	printf("Connected !\n");
	
	//hlavny cyklus zadavania prikazov
	while(koniec){
		scanf("%s", prikaz);
        	if (strcmp(prikaz, "make") == 0) {	//pripad pre prikaz make
			memset(file, 0, sizeof(file));
          		scanf("%s", file);		//nacitanie suboru
            	
            		if ((f = fopen(file, "r")) == NULL) {			//otvorenie suboru
                		fprintf(stderr, "File doesn\'t exist.\n");
                		continue;
            		}
            		write(client_socket, prikaz, sizeof(prikaz));		//poslanie prikazu make
			read(client_socket, prikaz, sizeof(prikaz));		//cakanie na odpoved
			send_file(client_socket, file);				//posielanie suboru
            		continue;
		}
		
		write(client_socket, prikaz, sizeof(prikaz));			//poslanie prikazu
		
		if(strcmp(prikaz, "halt") == 0) koniec = 0;	// ak je vstup halt tak koniec
		if(strcmp(prikaz, "quit") == 0) {		//ak je vstup quit tak ukoncit spojenie
			close(client_socket);
			while(koniec){				//cyklus pre ukoncenie programu alebo pripojenie
				printf("Type 'connect' to connect to server\nType 'exit' to exit the program\n");
				scanf("%s",prikaz);
				if(strcmp(prikaz, "connect") == 0){
					client_socket = socket(AF_INET, SOCK_STREAM, 0);
					if (client_socket == -1){
						fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

						exit(EXIT_FAILURE);
					}
					printf("IP to connect to: ");
					scanf("%s", ip_addr);
					inet_pton(AF_INET, ip_addr, &(remote_addr.sin_addr));
					if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1){
						fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

						exit(EXIT_FAILURE);
					}
					break;
				}
				else if(strcmp(prikaz, "exit") == 0){
					return;
					}
				else {
					printf("Unknown command\n");
				}
			}
		}
		
		memset(prikaz, 0, BUFF);	//vynulovanie
	}

	close(client_socket);
}


int main(int argc, char **argv) {
	char s = 0, t = 0, l = 0,  c = 0,d = 0;
    	if (handle_arguments(&s, &t, &l, &c, &d, argc, argv))
        return 1;
	
	//sravanie server/client podla argumentov pri spustani
    	if (s == 1 || (s == 0 && c == 0))
        server();
    	if (s == 0 && c == 1)
        client();

	return 0;
}
