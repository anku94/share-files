#ifndef LIBP2P_H
#define LIBP2P_H

#include "short_list.h"
#include "checksum.h"

#define TRUE 1
#define FALSE 0

void init()
{
	check = (char *)malloc(sizeof(char)*10000);
	fileNames = (char *)malloc(sizeof(char)*5000);
	fileNames1 = (char *)malloc(sizeof(char)*5000);
}
int queryType, sock;

char fileName[100];
char startStamp[100], endStamp[100];
char regex[100];

int getIndex(char tmp[]);
int wildcard(char *string, char *pattern);
char * regexList(char *dir, char *regex, int depth);
void parseRegex(char *string);
void execRecv(char *, int);
void queryOne();
void queryTwo();
void queryThree();
void queryFour();
void queryFive();
void execHandler();
int parseCmd(char *);
char *longList(char *, int);
void printLongList(char *);

char *hardPat = "*.txt";

int getIndex(char tmp[])
{
	int i;
	for(i=0;i<strlen(tmp);i++)
	{   
		if(tmp[i]==' ')
		{   
			return i;
			break;
		}   
	}   
}

int wildcard(char *string, char *pattern)
{
	//printf("WILDCARD %s %s\n", string, pattern);
	while(*string) 
	{   
		switch(*pattern)
		{   
			case '*': do 
				  {
					  ++pattern;
				  }
				  while(*pattern == '*');
					  if(!*pattern) return(TRUE);
				  while(*string)
				  {
					  if(wildcard(pattern,string++)==TRUE)
						  return(TRUE);}
				  return(FALSE);
			default : if(*string!=*pattern)return(FALSE); break;
		}   
		++pattern;
		++string;
	}   

	while (*pattern == '*') ++pattern;
	return !*pattern;
}



char * regexList(char *dir, char *regex, int depth)
{
	char *outputBuffer = (char *)malloc(sizeof(char)*1000);
	//memset(&outputBuffer, '\0', 1000);
	outputBuffer[0] = '\0';
	outputBuffer[1] = '\0';
	//printf("Regex is -> %s\n", regex);
	
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	int spaces = depth*4;
	int i;
	int flag=0;
	if((dp = opendir(dir)) == NULL) 
	{
		fprintf(stderr,"cannot open directory: %s\n", dir);
		return;
	}
	chdir(dir);
	while((entry = readdir(dp)) != NULL) 
	{
		lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)) 
		{
			/* Found a directory, but ignore . and .. */
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
			{
				continue;
			}
			//printf("%*s%s/\n",spaces,"",entry->d_name);
			flag=5;
			//regexList(entry->d_name,depth+1);
		}
		else 
		{

			//printf("%*s%s\n",spaces,"",entry->d_name);
		
		}
		char file[strlen(entry->d_name)+1];// = "dir.c";
		strcpy(file,entry->d_name);
		char t[100] = ""; 
		struct stat b;
		if (!stat(file, &b)) 
		{  
			strftime(t, 100, "%d/%m/%Y %H:%M:%S", localtime( &b.st_mtime));
			//printf("Last modified date and time = %s\n\n", t); 
			//printf("===> %s %s\n", entry->d_name, regex);
			if(wildcard(entry->d_name,regex)==TRUE)
			{
				if(flag==5)
				{
					strcat(outputBuffer," ");
					strcat(outputBuffer,entry->d_name);
					strcat(outputBuffer,"\n");
					//printf("It is a Directory \n");
					strcat(outputBuffer,"Directory\n");
					flag=0;
				}
				else
				{
					strcat(outputBuffer,entry->d_name);
					strcat(outputBuffer,"\n");
					//printf("It is a File\n");
					strcat(outputBuffer,"File\n");
				}
				strcat(outputBuffer,t);
				strcat(outputBuffer,"\n");
				int size = b.st_size;
				//printf("Size is %d\n",size);
				char str[15];
				sprintf(str, "%d",size);
				strcat(outputBuffer,str);
				strcat(outputBuffer," bytes");
				strcat(outputBuffer,"\n");
			}
		}   
		else 
		{   
			printf("Cannot display the time.\n");
		}   
	}
	chdir("..");
	closedir(dp);
	return outputBuffer;
}
void parseRegex(char *string)
{
	int i;
	int count = 0;
	for(i=0;i<strlen(string);i++)
	{
		if(count == 4)
		{
			count = 0;
			printf("\n");
		}
		if(string[i]=='\n' && string[i+1]==' ')
		{
			printf("\n\n Directory +++++++++++++++\n\n");          
		}
		else if(string[i] == '\n'){
			count++;
		 	printf("%c",string[i]);
		}
		else
		{
			printf("%c",string[i]);
		}
	}
}
void parse_check()
{
	NUMBER_OF_FILES_1=0;
	int i=0,k=0,y=0,flag=0,j=0;
	for(i=0;i<strlen(check);i++)
	{

		if(check[i]=='\n')
		{
			if(flag==0)
			{
				j=0;
				y++;
				flag++;
			}
			else if(flag==1)
			{
				j=0;
				k++;
				y=0;
				flag=0;
			}
		}
		if(flag==0)
		{
			temp1[k][y][j]=check[i];
		}
		else if(flag==1)
		{
			temp1[k][y][j]=check[i];
		}
		j++;
	}
	NUMBER_OF_FILES_1=k;
}
void execRecv(char *data, int dataLen)
{
	printf("Received some data %s\n", data);
	if(dataLen == 0)
		return;
	char tmp[dataLen + 3];
	strcpy(tmp, data);
	char *cur = strtok(tmp, "|");

	if(strcmp(cur, "cmd")==0){
		cur = strtok(NULL, "");
		printf("%s\n", cur);
		parseCmd(cur);
		execHandler();
	}
	else if(strcmp(cur, "pdl")==0)
	{
		cur = strtok(NULL, "");
		int size = atoi(cur);
		printf("Size: %d\n", size);
		char tmpData[size];
		int dataLeft = size;
		int dataReceived;

		send(sock, "|ack|", 5, 0);
		if(size > 0){

			while(1){
				if((dataReceived=recv(sock, tmpData, dataLeft, 0))!=-1){
					if(dataReceived != size){
						printf("\n\nIncomplete Transmission\n\n");
					}
					break;
				}
				usleep(10000);
			}
			tmpData[dataReceived] = '\0';
			//printf("%s\n", tmpData);
			parseRegex(tmpData);
		}

	}
	else if(strcmp(cur, "hsh")==0)
	{
		cur = strtok(NULL, "");
		int size = atoi(cur);
		printf("Size: %d\n", size);
		char tmpData[size + 50];
		char finData[size + 50];
		finData[0] = '\0';
		finData[1] = '\0';
		finData[2] = '\0';
		int totalGot = 0;
		int dataLeft = size;
		int dataReceived = 0;
		send(sock, "|ack|", 5, 0);
		char ack[10];
		if(size > 0)
		{
			while(1)
			{
				if((dataReceived = recv(sock, tmpData, dataLeft, 0))!=-1)
				{
					//printf("Got %d\n%s\n", dataReceived, tmpData);
					//printf("==> %d\n", dataReceived);
					strcat(finData, tmpData);
					totalGot += dataReceived;
					dataLeft -= dataReceived;
					if(totalGot == size)
					{
						break;
					}
					send(sock, "|ack|", 5, 0);
				}
			}
			finData[size] = '\0';

			//			printf("Debugging +++++++++++++++++++++++++++++++++++\n");
			//		printf("%s\n", finData);
			//			printf("Debugging +++++++++++++++++++++++++++++++++++\n");

			int i=0,k=0,y=0,flag=0,j=0;
			NUMBER_OF_FILES=0;
			//printf("finLen ===> %d\n", strlen(finData));
			for(i=0;i<strlen(finData);i++)
			{

				if(finData[i]=='\n')
				{
					if(flag==0)
					{
						j=0;
						y++;
						flag++;
					}
					else if(flag==1)
					{
						j=0;
						k++;
						y=0;
						flag=0;
					}
				}
				if(flag==0)
				{
					temp[k][y][j]=finData[i];
				}
				else if(flag==1)
				{
					temp[k][y][j]=finData[i];
				}
				j++;
			}
			NUMBER_OF_FILES=k;
			//printf("try to check  %d %d\n",NUMBER_OF_FILES,NUMBER_OF_FILES_1);

			int hashLen = strlen(check);

			check[0]='\0';
			getHash1("./share", 0);
			hashLen = strlen(check);

			NUMBER_OF_FILES_1=0;
			//			printf("Anirudh Goyal q4 --> %d\n", hashLen);
			//printf("%s\n", check);
			parse_check();

			/*printf("Starting Parse Structure +++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			  for(i=0;i<NUMBER_OF_FILES;i++)             
			  {
			  for(k=0;k<2;k++)
			  {
			  printf("%s",temp[i][k]);
			  }
			  printf(" %d\n",i);
			  }
			  printf("Ending Parse Structure ++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			  */
			/*
			   printf("Starting Parse Structure +++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			   for(i=0;i<NUMBER_OF_FILES_1;i++)             
			   {
			   for(k=0;k<2;k++)
			   {
			   printf("%s",temp1[i][k]);
			   }
			   printf(" %d\n",i);
			   }
			   printf("Ending Parse Structure ++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			   */
			//printf("Mistake %d %d\n",NUMBER_OF_FILES,NUMBER_OF_FILES_1);
			for(i=0;i<NUMBER_OF_FILES;i++)
			{
				for(j=0;j<NUMBER_OF_FILES_1;j++)
				{
					if(!(strcmp(temp[i][0],temp1[j][0])))
					{
						if(!(strcmp(temp[i][1],temp1[j][1])))
						{
							printf("%s\n",temp[i][0]);
							printf("Contents have not been changed.\n");
						}
						else
						{
							printf("%s\n",temp[i][0]);
							printf("Contents have been changed.\n");
						}
					}
				}
			}

		}
	}
	else if(strcmp(cur, "file")==0)
	{
		printf("Happy\n");
		cur = strtok(NULL, "");
		printf("Ensured Size: %s\n",cur);

		char resp, back;
		printf("A file is available for download. Download? (Y/N): ");
		fflush(stdout);
		while(1){
			if(read(0, &resp, 1)!=-1){
				break;
			}
		}
		read(0, &back, 1);
		printf("Resp: %c\n", resp);

		if(resp == 'Y' || resp =='y' ){
			send(sock, "|ack|", 5, 0);
		}
		else{
			send( sock, "|nak|", 5, 0);
			return;
		}

		int dataReceived;
		char finalPath[1000];
		finalPath[0]='\0';
		sprintf(finalPath, "share/%s", cur);
		FILE *f1=fopen(finalPath,"wb");
		int dataRecieved;
		char recv_d[1024+10];

		while (1)
		{
			if((dataRecieved = recv(sock,recv_d,1024,0))!=-1)
			{
				printf(" RECEIVING DATA...%d\n",dataRecieved);
				recv_d[dataRecieved] = '\0';
				printf("ANisa %d\n",dataRecieved);
				fwrite(recv_d,1,dataRecieved,f1);
				send(sock, "|ack|", 5, 0);
				fflush(stdout);
				if(dataRecieved < 1024)
				{
					//	send(sock, "|ack|", 5, 0);
					break;
				}
			}
			usleep(500);
		}
		fclose(f1);

	}
	else if(strcmp(cur, "ssl")==0)
	{
		cur = strtok(NULL, "");
		int dataReceived;
		size_t mdsize = atoi(cur);
		//printf("%d\n", mdsize);
		char thisHash[2*MD5_DIGEST_LENGTH+1];
		char otherHash[2*MD5_DIGEST_LENGTH+1];
		send(sock, "|ack|", 5, 0);
		while(1){
			if((dataReceived = recv(sock, otherHash, mdsize, 0))!=-1)
			{
				if(dataReceived < mdsize){
					printf("Insufficient transmission\n");
				}
				break;
			}
			usleep(10000);
		}
		otherHash[dataReceived]='\0';
		//printf("Hash ===> %s\n", otherHash);
		char finalPath[1000];
		sprintf(finalPath, "share/%s", fileName);
		getMD5(finalPath);
		strcpy(thisHash, compute_checksum_2);
		//printf("Hash other ==> %s\n", compute_checksum_2);
		if(strcmp(thisHash, otherHash)==0){
			printf("Contents unchanged.\n");
		}
		else{
			printf("Contents have changed. Please download again.\n");
		}
	}

}
void queryFive()
{
	printf("have fun\n");
	char finalPath[1000];
	finalPath[0] = '\0';
	finalPath[1] = '\0';
	sprintf(finalPath, "share/%s", fileName);
	FILE *f = fopen(finalPath,"rb");
	int rcvdData = 0;
	if(f!=NULL)
	{
		fseek(f,0,SEEK_END);
		int size=ftell(f);
		char pdl[50];
		char pdl2[20];
		char send_d[1024];
		sprintf(pdl, "file|%s",fileName );
		printf("Are you listing %s %s\n",fileName,pdl);
		send(sock, pdl, strlen(pdl), 0);
		while(1)
		{
			if((rcvdData = recv(sock, pdl2, 20, 0))!=-1)
			{
				pdl2[rcvdData] = '\0';
				if(strcmp(pdl2, "|nak|")==0){
					printf("Refused file transfer.\n");
					fclose(f);
					return;
				}
				if(strcmp(pdl2, "|ack|")!=0)
				{
					printf("\n\nVerification error\n\n");
				}
				else
					printf("Ani-Anky\n");
				break;
			}
		}
		fseek(f,0,SEEK_SET);
		int xx;
		while(1)
		{
			if(!feof(f))
			{
				xx=fread(send_d,1,1024,f);
				//printf("length of data %d %s\n",xx,send_d);
			}
			else
			{
				break;
			}
			send(sock,send_d,xx, 0);
			while(1)
			{
				if((rcvdData = recv(sock, pdl2, 20, 0))!=-1)
				{
					pdl2[rcvdData] = '\0';
					if(strcmp(pdl2, "|ack|")!=0)
					{
						printf("\n\nVerification error\n\n");
					}
					break;
				}
			}
		}
		if (!ferror(f))
		{
			//perror("Error \n");
		}
		
	}
	else
		return;
	fclose(f);
}
void queryFour()
{
	//Take a structure of all downloaded files and their hashes, convert to string and send to other peer
	//printdir->getHash
	check[0] = '\0';
	check[1] = '\0';
	getHash("./share", 0);

	//Check structure has filenames and hashes
	
	int hashLen = strlen(check);

//	printf("q4 --> %d\n", hashLen);

//	printf("%s\n", check);

	char lenPack[10];

	sprintf(lenPack, "hsh|%d", hashLen);
	send(sock, lenPack, strlen(lenPack), 0);

	char pdl[10];
	pdl[0] = '\0';
	int rcvdData;


	while(1){
		if((rcvdData = recv(sock, pdl, 20, 0))!=-1){
			pdl[rcvdData] = '\0';
			if(strcmp(pdl, "|ack|")!=0){
				printf("\n\nVerification error\n\n");
			}
			break;
		}
		usleep(10000);
	}
	printf("Got acknowledgement");
	int sentData;
	int totalSent = 0;

	while(1){
		sentData = send(sock, &check[totalSent], hashLen, 0);
		printf("Sent - %d\n", sentData);
		totalSent += sentData;
		if(totalSent >= hashLen)
			break;
		while(1){
			if((rcvdData = recv(sock, pdl, 20, 0))!=-1){
				pdl[rcvdData] = '\0';
				if(strcmp(pdl, "|ack|")!=0){
					printf("\n\nVerification error\n\n");
				}
				break;
			}
			usleep(10000);
		}
	}
	printf("Finished\n");
}
void queryZero(){
	//printf("Regex is: %s\n", regex);
	char *temp = NULL;
	//temp = shortList("./share", "28/03/2012 10:50:56", "30/04/2013 00:22:52", 0); 
	temp = shortList("./share", startStamp, endStamp, 0); 
	printf("=======\n%s\n=======\n", temp);
	int tempSize = strlen(temp);
	char *cur;
	int sentData = 0;
	int rcvdData = 0;
	char pdl[20];
	sprintf(pdl, "pdl|%d", tempSize);
	send(sock, pdl, strlen(pdl), 0);
	//printf("%s\n", temp);
	while(1){
		if((rcvdData = recv(sock, pdl, 20, 0))!=-1){
			pdl[rcvdData] = '\0';
			if(strcmp(pdl, "|ack|")!=0){
				printf("\n\nVerification error\n\n");
			}
			break;
		}
		usleep(10000);
	}
	sentData = send(sock, temp, strlen(temp), 0);

	if(sentData != tempSize){
		printf("\n\nIncomplete transmission...\n\n");
	}
	
	free(temp);
}
void queryThree(){
	//char *temp = longList("./share", 0);
	//fileName
	char finalPath[1000];
	finalPath[0] = '\0';
	finalPath[1] = '\0';
	
	sprintf(finalPath, "share/%s", fileName);

	char temp[2*MD5_DIGEST_LENGTH+1];

	getMD5(finalPath);
	strcpy(temp, compute_checksum_2);

	printf("MD5 HASH --> %s\n", temp);

	int tempSize = strlen(temp);
	char *cur;
	int sentData = 0;
	int rcvdData = 0;
	char pdl[20];
	sprintf(pdl, "ssl|%d", tempSize);
	send(sock, pdl, strlen(pdl), 0);
	//printf("%s\n", temp);
	while(1){
		if((rcvdData = recv(sock, pdl, 20, 0))!=-1){
			pdl[rcvdData] = '\0';
			if(strcmp(pdl, "|ack|")!=0){
				printf("\n\nVerification error\n\n");
			}
			break;
		}
		usleep(10000);
	}
	sentData = send(sock, temp, strlen(temp), 0);

	if(sentData != tempSize){
		printf("\n\nIncomplete transmission...\n\n");
	}
}
void queryTwo(){
	char tmpRegex[100];
	int count = 0;
	int i;
	for(i=0;i<strlen(regex);i++){
		if(regex[i] != '"'){
			tmpRegex[count++]=regex[i];
		}
	}
	tmpRegex[count]='\0';
	strcpy(regex, tmpRegex);
	//printf("Regex is: %s\n", regex);
	char *temp = regexList("./share", regex, 0);
	//printf("=======\n%s\n=======\n", temp);
	int tempSize = strlen(temp);
	char *cur;
	int sentData = 0;
	int rcvdData = 0;
	char pdl[20];
	sprintf(pdl, "pdl|%d", tempSize);
	send(sock, pdl, strlen(pdl), 0);
	//printf("%s\n", temp);
	while(1){
		if((rcvdData = recv(sock, pdl, 20, 0))!=-1){
			pdl[rcvdData] = '\0';
			if(strcmp(pdl, "|ack|")!=0){
				printf("\n\nVerification error\n\n");
			}
			break;
		}
		usleep(10000);
	}
	sentData = send(sock, temp, strlen(temp), 0);

	if(sentData != tempSize){
		printf("\n\nIncomplete transmission...\n\n");
	}
	
	free(temp);
}
void queryOne()
{
	char *temp = longList("./share", 0);
	int tempSize = strlen(temp);
	char *cur;
	int sentData = 0;
	int rcvdData = 0;
	char pdl[20];
	sprintf(pdl, "pdl|%d", tempSize);
	send(sock, pdl, strlen(pdl), 0);
	//printf("%s\n", temp);
	while(1){
		if((rcvdData = recv(sock, pdl, 20, 0))!=-1){
			pdl[rcvdData] = '\0';
			if(strcmp(pdl, "|ack|")!=0){
				printf("\n\nVerification error\n\n");
			}
			break;
		}
		usleep(10000);
	}
	sentData = send(sock, temp, strlen(temp), 0);

	if(sentData != tempSize){
		printf("\n\nIncomplete transmission...\n\n");
	}
	
	free(temp);
}
void execHandler(){
	if(queryType < 0 || queryType > 6){
		printf("Invalid query.\n");
		return;
	}
	switch(queryType){
		case 0:
			//printf("To be executed soon...\n");
			queryZero();
			break;
		case 1:
			queryOne();
			break;
		case 2:
			queryTwo();
			break;
		case 3:
			queryThree();
			break;
		case 4:
			queryFour();
			break;
		case 5:
			queryFive();
			break;

	}
}

int parseCmd(char *cmd){
	char *cur;
	queryType = -1;

	cur = strtok(cmd, " ");

	if(cur == NULL)
		return -1;

	if(strcmp(cur, "IndexGet")==0){
		cur = strtok(NULL, " ");
		if(cur == NULL)
			return -1;
		if(strcmp(cur, "ShortList")==0){
			cur = strtok(NULL, " ");
			if(cur == NULL)
				return -1;
			strcpy(startStamp, cur);
			
			cur = strtok(NULL, " ");
			if(cur == NULL)
				return -1;
			strcpy(endStamp, cur);

			queryType = 0;
			printf("Dates: %s %s\n", startStamp, endStamp);

			return 0;
		}
		else if(strcmp(cur, "LongList")==0){
			queryType = 1;
			return 0;
		}
		else if(strcmp(cur, "RegEx")==0){
			cur = strtok(NULL, " ");
			if(cur == NULL)
				return -1;
			strcpy(regex, cur);
			queryType = 2;
			printf("Regex: %s\n", regex);
			return 0;
		}
		else{
			return -1;
		}
	}
	else if(strcmp(cur, "FileHash")==0){
		cur = strtok(NULL, " ");
		if(cur == NULL)
			return -1;

		if(strcmp(cur, "Verify")==0){
			cur = strtok(NULL, " ");
			if(cur == NULL)
				return -1;
			strcpy(fileName, cur);
			printf("File: %s\n", fileName);
			queryType = 3;
			return 0;
		}
		else if(strcmp(cur, "CheckAll")==0){
			queryType = 4;
			return 0;
		}
		else{
			return -1;
		}
	}
	else if(strcmp(cur, "FileDownload")==0){
		cur = strtok(NULL, " ");
		if(cur == NULL)
			return -1;
		else{
			strcpy(fileName, cur);
			printf("File: %s\n", fileName);
			queryType = 5;
			return 0;
		}

	}
	else if(strcmp(cur, "FileUpload")==0)
	{
		cur = strtok(NULL, " ");
		if(cur == NULL)
			return -1;
		else
		{
			strcpy(fileName, cur);
			queryType = 6;
			return 0;
		}
	}
	else
	{
		return -1;
	}
}
char *longList(char *dir, int depth)//11,84
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	int spaces = depth*4;
	int i;
	int flag=0;
	char *outString = (char *)malloc(sizeof(char)*1000);
	outString[0] = '\0';
	outString[1] = '\0';
	if((dp = opendir(dir)) == NULL) 
	{
		fprintf(stderr,"cannot open directory: %s\n", dir);
		return;
	}
	chdir(dir);
	while((entry = readdir(dp)) != NULL) 
	{
		lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)) 
		{
			/* Found a directory, but ignore . and .. */
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
			{
				continue;
			}
			strcat(outString," ");
			strcat(outString,entry->d_name);
			strcat(outString,"\n");
			//printf("%*s%s/\n",spaces,"",entry->d_name);
			flag=5;
			//longList(entry->d_name,depth+1);
		}
		else 
		{
			strcat(outString,entry->d_name);
			strcat(outString,"\n");
			//printf("%*s%s\n",spaces,"",entry->d_name);
		}
		char file[strlen(entry->d_name)+1];// = "dir.c";
		strcpy(file,entry->d_name);
		char t[100] = ""; 
		struct stat b;
		if(flag==5)
		{
			//printf("It is a Directory \n");
			strcat(outString,"Directory\n");
			flag=0;
		}
		else
		{
			//printf("It is a File\n");
			strcat(outString,"File\n");
		}
		if (!stat(file, &b)) 
		{  
			int size = b.st_size;
			//printf("Size is %d\n",size);
			char str[15];
			sprintf(str, "%d",size);
			strcat(outString,str);
			strcat(outString," bytes");
			strcat(outString,"\n");
			strftime(t, 100, "%d/%m/%Y %H:%M:%S", localtime( &b.st_mtime));
			//printf("Last modified date and time = %s\n\n", t); 
			strcat(outString,t);
			strcat(outString,"\n");
		}   
		else 
		{   
			printf("Cannot display the time.\n");
		}   
	}
	chdir("..");
	closedir(dp);
	return outString;
}
void printLongList(char *string)
{
	int i, count=0;
	for(i=0;i<strlen(string);i++)
	{
		if(string[i] == '\n')
			count++;
		if(count == 4)
		{
			count = 0;
			printf("\n");
		}
		if(string[i]=='\n' && string[i+1]==' ')
		{
			printf("\n\n Directory +++++++++++++++\n\n");          
		}
		else
		{
		 	printf("%c",string[i]);
		}
	}
}

#endif
