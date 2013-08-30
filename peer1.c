/* tcpserver.c */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include "libp2p.h"

int main(int argc, char *argv[])
{
	init();
	int listenSock, bytes_recieved , true = 1;  
	char send_data [1024] , recv_data[1024];
 	int sizeRead = 0;
	char c;	

	struct sockaddr_in server_addr,client_addr;    
	int sin_size;

	fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

	if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}


	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(5000);     
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 

	if (bind(listenSock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
			== -1) {
		perror("Unable to bind");
		exit(1);
	}

	if (listen(listenSock, 5) == -1) {
		perror("Listen");
		exit(1);
	}

	printf("\nTCPServer Waiting for client on port %d\n", ntohs(server_addr.sin_port));
	fflush(stdout);


	while(1)
	{  

		sin_size = sizeof(struct sockaddr_in);

		sock = accept(listenSock, (struct sockaddr *)&client_addr,&sin_size);
		fcntl(sock, F_SETFL, O_NONBLOCK);

		printf("\n I got a connection from (%s , %d)\n",
				inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		
		printf("P2PCMD : ");
		fflush(stdout);

		while (1)
		{
			sizeRead = 0;

			if(read(0, &c, 1) != -1){

				while(c != '\n'){
					send_data[sizeRead] = c;
					sizeRead++;
					read(0, &c, 1);
				}
				send_data[sizeRead] = '\0';

				if ((strcmp(send_data , "q") == 0) || (strcmp(send_data , "Q") == 0))
				{
					send(sock, send_data,strlen(send_data), 0);
					close(sock);
					break;
				}

				else{
					char *temp = (char *)malloc(sizeof(char)*(strlen(send_data)+10));
					strcpy(temp, "cmd|");
					strcat(temp, send_data);
					parseCmd(send_data);
					if(queryType == -1){
						fprintf(stderr, "Invalid command\n");
					}
					else if(queryType == 6){
						queryFive();
					}
					else{
						send(sock, temp,strlen(temp), 0);
					}
					free(temp);
				}
				/*if(parseCmd(send_data) == 0){
					execHandler();
				}
				else{
					printf("Invalid command.\n");
				}*/
				printf("P2PCMD : ");
				fflush(stdout);
			}


			if((bytes_recieved = recv(sock,recv_data,1024,0)) != -1){

				recv_data[bytes_recieved] = '\0';

				if (strcmp(recv_data , "q") == 0 || strcmp(recv_data , "Q") == 0)
				{
					close(sock);
					break;
				}

				else{ 
					//printf("\n RECIEVED DATA = %s\n" , recv_data);
					execRecv(recv_data, bytes_recieved);
				}
				printf("P2PCMD : ");
				fflush(stdout);
			}
			usleep(10000);
		}
		break;
	}
 	printf("Exiting P2PCMD...\n");	

	close(listenSock);
	return 0;
} 
