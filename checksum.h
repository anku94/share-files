#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include<stdlib.h>
#include<time.h>
#include <openssl/md5.h>
#include<errno.h>

char *fileNames;
char *fileNames1;
unsigned char *check;
char **share;
char temp[100][2][100];  // 3-D structure comprtising of the files and their hash values as sent by the peer2
char temp1[100][2][100];  // 3-D structure comprising of the files and their hash values which are downloaded by the peer1.
int NUMBER_OF_FILES=0;
int NUMBER_OF_FILES_1=0;
int NUMBER_OF_DOWNLOADED_FILES=0;

char directory[]="." ;   // update the name of the folder in which files are to be kept

char compute_checksum[2*MD5_DIGEST_LENGTH+1];
char compute_checksum_2[2*MD5_DIGEST_LENGTH+1];
/*
*/
void compute(char *filename,int verbose,char *str)
{
	//printf("%s\n",filename);
	unsigned char c[MD5_DIGEST_LENGTH];
	int i;
	char d[2*MD5_DIGEST_LENGTH+1];
	char buff[1000];
	
	FILE *inFile;
	char file_location[100];
	
	if(verbose==0)
	{
		strcpy(file_location,directory);
		strcat(file_location,"/");
		strcat(file_location,filename);
		inFile = fopen (file_location, "rb");
	}
	else if(verbose==5)
	{
		inFile = fopen (filename, "rb");
	}
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	if (inFile == NULL) 
	{
		printf ("%s can't be opened.\n", filename);
		return ;
	}   
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, inFile)) != 0)
	{
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (c,&mdContext);

	for(i = 0; i < MD5_DIGEST_LENGTH; i++) 
	{
		sprintf(&d[i*2], "%02x", c[i]);
	}
//	printf ("ANirudh %s\n", filename);
	if(verbose==5)
	{
		strcpy(compute_checksum,d);
	//	printf("%s\n",compute_checksum);
	//	printf("%s %s\n",compute_checksum,str);
		if(!(strcmp(d,str)))
		{
			printf("Mast +++++++++++++++++++++++++++++++++++++++%s\n",compute_checksum);
		}
		else
		{
			printf("File has to be downloaded again \n");
		}
	}
	if(verbose==0)
	{
		strcat(check,filename);
		strcat(check,"\n");
		strcat(check,d);
		strcat(check,"\n");
	}
	fclose (inFile);
}

void getMD5(char *filename)
{
	unsigned char c[MD5_DIGEST_LENGTH];
	int i;
	char d[2*MD5_DIGEST_LENGTH+1];
	char buff[1000];
	
	FILE *inFile;
	char file_location[100];
	
//	if(verbose==0)
//	{
		strcpy(file_location,directory);
		strcat(file_location,"/");
		strcat(file_location,filename);
		inFile = fopen (file_location, "rb");
//	}
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	if (inFile == NULL) 
	{
		printf ("%s can't be opened.\n", filename);
		return ;
	}   
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, inFile)) != 0)
	{
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (c,&mdContext);

	for(i = 0; i < MD5_DIGEST_LENGTH; i++) 
	{
		sprintf(&d[i*2], "%02x", c[i]);
	}
//	if(verbose==0)
//	{
		strcpy(compute_checksum_2,d);
		//compute_checksum_2[MD5_DIGEST_LENGTH]='\0';
	//	printf("%s\n",compute_checksum);
	//	printf("%s %s\n",compute_checksum,str);
	/*	if(!(strcmp(d,str)))
		{
			printf("Mast +++++++++++++++++++++++++++++++++++++++%s\n",compute_checksum);
		}
		else
		{
			printf("File has to be downloaded again \n");
		}
	*/	
//	}
	fclose (inFile);
}
void getHash(char *dir, int depth)
{
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
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
			{
				continue;
			}
			strcat(fileNames,entry->d_name);
			strcat(fileNames,"\n");
			flag=5;
		}
		else 
		{
			strcat(fileNames," ");
			strcat(fileNames,entry->d_name);
			strcat(fileNames,"\n");
			compute(entry->d_name,0,"fileNames-anky");
		}
	}
	chdir("..");
	closedir(dp);
}
void getHash1(char *dir, int depth)
{
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
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
			{
				continue;
			}
			strcat(fileNames1,entry->d_name);
			strcat(fileNames1,"\n");
			flag=5;
		}
		else 
		{
			strcat(fileNames1," ");
			strcat(fileNames1,entry->d_name);
			strcat(fileNames1,"\n");
			compute(entry->d_name,0,"fileNames-anky");
		}
	}
	chdir("..");

	closedir(dp);
}
// It parses the array given by the peer2 in 3-D structure ....
void parse_checksum()
{
	NUMBER_OF_FILES=0;
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
			temp[k][y][j]=check[i];
		}
		else if(flag==1)
		{
			temp[k][y][j]=check[i];
		}
		j++;
	}
	NUMBER_OF_FILES=k;
}


void compare_checksum()
{
	int i,k=0,j;
	for(i=0;i<NUMBER_OF_FILES;i++)
	{
		k=0;
		char a[strlen(temp[i][0])];
		char b[strlen(temp[i][1])];
		for(j=0;j<strlen(temp[i][0]);j++)
		{
			if(temp[i][0][j]!='\n')
			{
				a[k]=temp[i][0][j];
				k++;
			}
		}
		a[k]='\0';
		k=0;
		for(j=0;j<strlen(temp[i][1]);j++)
		{
			if(temp[i][1][j]!='\n')
			{
				b[k]=temp[i][1][j];
				k++;
			}
		}
		b[k]='\0';
		compute(a,5,b);
	}	

}

/*
int main(int argc, char* argv[])
{
	char *topdir, pwd[3]=".";
	if (argc != 2)
	{
		topdir=pwd;
	}
	else
	{
		topdir=argv[1];
	}
	printf("Directory scan of %s\n",topdir);
	fileNames=malloc(sizeof(char)*1000);
	check=malloc(sizeof(char)*4000);
	getHash(topdir,0);
	printf("done.\n");
	int i;
	int flag=0;
	int k=0;
	parse_checksum();			// Another peer parses into 3-D structure comprising of the file_names and their checksum values
	
	// Comprising of the file name and it's hash corresponding to the file name :-
	for(i=0;i<NUMBER_OF_FILES;i++)			// NUMBER_OF_FILES  count of files in  a directory.
	{
		for(k=0;k<2;k++)
		{
			printf("%s",temp[i][k]);
		}
		printf(" %d\n",i);
	}
	printf("%d %d %d\n",strlen(fileNames),strlen(check),NUMBER_OF_FILES);
	
	int j=0;	
	compare_checksum();
	return 0;
}*/
#endif
