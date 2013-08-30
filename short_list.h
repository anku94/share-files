#ifndef SHORTLIST_H
#define SHORTLIST_H
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include<stdlib.h>
#include<time.h>
#include <openssl/md5.h>
char *ani;


int get_index(char tmp[])
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


int compareFunc(char tmp[],char tmp1[],char tmp2[])
{
	int a=get_index(tmp);
	int b=get_index(tmp1);
	int c=get_index(tmp2);
	char A[strlen(tmp)];
	char B[strlen(tmp1)+1];
	char C[strlen(tmp2)+1];
	int i;
	A[0]=tmp[6];
	A[1]=tmp[7];
	A[2]=tmp[8];
	A[3]=tmp[9];
	A[4]='/';
	A[5]=tmp[3];
	A[6]=tmp[4];
	A[7]='/';
	A[8]=tmp[0];
	A[9]=tmp[1];
	/*for(i=a;i<strlen(tmp);i++)
	  {
	  A[i]=tmp[i];
	  }
	 */
	A[10]='\0';
	//printf("Anirudh %s\n",A);
	B[0]=tmp1[6];
	B[1]=tmp1[7];
	B[2]=tmp1[8];
	B[3]=tmp1[9];
	B[4]='/';
	B[5]=tmp1[3];
	B[6]=tmp1[4];
	B[7]='/';
	B[8]=tmp1[0];
	B[9]=tmp1[1];
	/*for(i=b;i<strlen(tmp);i++)
	  {
	  B[i]=tmp1[i];
	  }
	 */
	B[10]='\0';

	//printf("Hey Anirudh %s\n",B);
	C[0]=tmp2[6];
	C[1]=tmp2[7];
	C[2]=tmp2[8];
	C[3]=tmp2[9];
	C[4]='/';
	C[5]=tmp2[3];
	C[6]=tmp2[4];
	C[7]='/';
	C[8]=tmp2[0];
	C[9]=tmp2[1];
	/*for(i=c;i<strlen(tmp2);i++)
	  {
	  C[i]=tmp2[i];
	  }
	 */
	C[10]='\0';
	//printf("Hey Anirudh %s\n",C);

	int fir=strcmp(A,B);
	int sec=strcmp(B,C);
	if(fir <= 0 && sec <=0)
	{
		return 5;
		printf("yes\n");
	}
	else 
		return 0;
	//printf("%d %d\n",fir,sec);


}



char * shortList(char *dir, char *startStamp, char *endStamp, int depth)//104,
{
	char *outputBuffer = (char *)malloc(sizeof(char)*1000);
	outputBuffer[0] = '\0';
	outputBuffer[1] = '\0';
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
	//		strcat(outputBuffer," ");
	//		strcat(outputBuffer,entry->d_name);
	//		strcat(outputBuffer,"\n");
			//printf("%*s%s/\n",spaces,"",entry->d_name);
			flag=5;
			//shortList(entry->d_name,depth+1);
		}
		else 
		{

	//		strcat(outputBuffer,entry->d_name);
	//		strcat(outputBuffer,"\n");
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
			//int ans=compareFunc("28/03/2013 10:50:56",t, "30/04/2013 00:22:52");
			int ans=compareFunc(startStamp,t, endStamp);
			if(ans==5)
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
/*void parse()
{
	int i;
	for(i=0;i<strlen(ani);i++)
	{
		if(ani[i]=='\n' && ani[i+1]==' ')
		{
			printf("\n\n Directory +++++++++++++++\n\n");          
		}
		else
		{
		 	printf("%c",ani[i]);
		}
	}
}
int main(int argc, char* argv[])
{
	//printf("Directory scan of %s\n",topdir);
	ani=shortList(argv[1], "28/03/2013 10:50:56", "30/03/2013 00:22:52", 0);
	parse();
	return 0;
}*/
#endif
