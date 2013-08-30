/* tcpclient.c */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include "libp2p.h"


int main()

{

	init();
	int bytes_recieved;  
	char send_data[1024],recv_data[1024];
	char c;
	int sizeRead;
	struct hostent *host;
	struct sockaddr_in server_addr;  

	host = gethostbyname("127.0.0.1");

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;     
	server_addr.sin_port = htons(5000);   
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8); 

	if (connect(sock, (struct sockaddr *)&server_addr,
				sizeof(struct sockaddr)) == -1) 
	{
		perror("Connect");
		exit(1);
	}
	fcntl(sock, F_SETFL, O_NONBLOCK);
	fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
	
	printf("\nP2PCMD : ");
	fflush(stdout);

	while(1)
	{
		sizeRead = 0;

		if((bytes_recieved=recv(sock,recv_data,1024,0)) != -1){
			recv_data[bytes_recieved] = '\0';

			if (strcmp(recv_data , "q") == 0 || strcmp(recv_data , "Q") == 0)
			{
				close(sock);
				break;
			}

			else{
				//printf("\nRecieved data = %s " , recv_data);
				execRecv(recv_data, bytes_recieved);
			}

			printf("\nP2PCMD : ");
			fflush(stdout);
		}
		if(read(0, &c, 1) != -1){

			while(c != '\n'){
				send_data[sizeRead] = c;
				sizeRead++;
				read(0, &c, 1); 
			}   
			send_data[sizeRead] = '\0';


			if (strcmp(send_data , "q") != 0 && strcmp(send_data , "Q") != 0){
				char *temp = (char *)malloc(sizeof(char)*(strlen(send_data)+10));
				strcpy(temp, "cmd|");
				strcat(temp, send_data);
				parseCmd(send_data);
				if(queryType == -1){
					fprintf(stderr, "Invalid query.\n");
				}
				else if(queryType == 6){
					queryFive();
				}
				else{
					send(sock,temp,strlen(temp), 0); 
				}
				free(temp);
			}
			else
			{
				send(sock,send_data,strlen(send_data), 0);   
				close(sock);
				break;
			}
			/*if(parseCmd(send_data)==0){
				execHandler();
			}
			else{
				printf("Invalid command.\n");
			}*/
			printf("\nP2PCMD : ");
			fflush(stdout);
		}
		usleep(10000);

	}   
	return 0;
}
