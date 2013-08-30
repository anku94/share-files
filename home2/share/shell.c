#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<ctype.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>

void sigchld_handler(int);
void print_promt();
void exec_command(int,int);

struct proc_list{
    int pid;
    int is_running;
    char name[100];
};

struct proc_list process[100];
int proc_count=0,foreground_running;
char home[100],command[1000],elfname[100];;
FILE *myhist;
int b_stdout,b_stdin,b_stderr;

void print_promt(){
    char username[100],hostname[100],pwd[100];
    char temp[100];
    if(getlogin_r(username,100)){
        printf("Get login method failed. Exiting\n");
        exit(1);
    }
    if(gethostname(hostname,100)){
        printf("Get host name method failed. Exiting\n");
        exit(1);
    }
    if(getcwd(pwd,100)==NULL){
        printf("Get current working directory method failed. Exiting\n");
        exit(1);
    }
    //  home=getenv("HOME");
    //  if(home==NULL){
    //      printf("Get home directory environment variable not defined. Exiting\n");
    //      exit(1);
    //  }
    if(!strncmp(pwd,home,strlen(home)-1)){
        strcpy(temp,&pwd[strlen(home)]);
        strcpy(pwd,"~");
        strcat(pwd,temp);
    }
    printf("<%s@%s:%s> ",username,hostname,pwd);
    return;
}

void sigchld_handler(int sig){
    pid_t pid;
    if(!foreground_running){
        pid = wait(NULL);
        int i;
        for(i=0;process[i].pid!=pid && i<proc_count;i++);
        if(i<proc_count){
            process[i].is_running=0;
            printf("\n%s exited successfully\n",process[i].name);
            print_promt();
        }
    }
    signal(SIGCHLD,sigchld_handler);
    return;
}

void exec_command(int start_pos,int input_stream){
	if(start_pos>strlen(command))return;
	while(isspace(start_pos))start_pos++;
	dup2(input_stream,0);
	int i,j,background_process,child_id,count,num,end_pos;
	char *args[100];
	char filename[100],temp[100];
	int input_file=0,output_file=0,outputerr_file=0;
        background_process=0;

	for(end_pos=start_pos;command[end_pos]!='\0' && command[end_pos]!='|';end_pos++);

        if(command[end_pos]=='&'){
            background_process=1;
            command[end_pos]=' ';
        }
	int flag=0;
        for(i=start_pos,num=start_pos,count=0;i<=end_pos;){
	     if(command[i]=='<'){
		i++;
		for(;isspace(command[i]);i++);
		for(j=0;!isspace(command[i]) && i<end_pos;i++,j++)filename[j]=command[i];
		filename[j]='\0';
		if(input_file!=0)close(input_file);
		input_file=open(filename,O_RDONLY,S_IRUSR|S_IWUSR);
		if(input_file==-1){
			printf("File Not Found\n");
			return;
		}
		dup2(input_file,0);
		for(;isspace(command[i]) && i<end_pos;i++);
		num=i;
	    }
	    else if(command[i]=='2'&&command[i+1]=='>'){
		i+=2;
		for(;isspace(command[i]);i++);
		for(j=0;!isspace(command[i]) && i<end_pos;i++,j++)filename[j]=command[i];
		filename[j]='\0';
		if(outputerr_file!=0)close(outputerr_file);
		outputerr_file=open(filename,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
		if(outputerr_file==-1){
			printf("Error Opening File\n");
			return;
		}
		dup2(outputerr_file,2);
		for(;isspace(command[i]) && i<end_pos;i++);
		num=i;
	    }
	    else if(command[i]=='>'&&command[i+1]=='>'){
		i+=2;
		for(;isspace(command[i]);i++);
		for(j=0;!isspace(command[i]) && i<end_pos;i++,j++)filename[j]=command[i];
		filename[j]='\0';
		if(output_file!=0)close(output_file);
		output_file=open(filename,O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR);
		if(output_file==-1){
			printf("Error Opening File\n");
			return;
		}
		dup2(output_file,1);
		for(;isspace(command[i]) && i<end_pos;i++);
		num=i;
	    }
	    else if(command[i]=='>'){
		i++;
		for(;isspace(command[i]);i++);
		for(j=0;!isspace(command[i]) && i<end_pos;i++,j++)filename[j]=command[i];
		filename[j]='\0';
		if(output_file!=0)close(output_file);
		output_file=open(filename,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
		if(output_file==-1){
			printf("Error Opening File\n");
			return;
		}
		dup2(output_file,1);
		for(;isspace(command[i]) && i<end_pos;i++);
		num=i;
	    }
            else if((isspace(command[i]) || i==end_pos)&&flag==1){
                args[count]=(char *)malloc(sizeof(char)*(i-num+1));
                strncpy(args[count],&command[num],i-num);
                strcat(args[count],"\0");
                count++;
                while(isspace(command[i]) && i<end_pos)i++;
		num=i;
		flag=0;
            }
	    else if(isspace(command[i]) && flag==0){
		i++;
		num=i;
		flag=0;
	    }
	    else{
		i++;
		flag=1;
	    }
        }
        args[count]=NULL;
	int mypipe[2];
	pipe(mypipe);

	if(command[end_pos]=='|'){
		dup2(mypipe[1],1);
	}
	else
		dup2(b_stdout,1);


	if(!strncmp(args[0],"hist",4)){
		if(!strcmp(args[0],"hist")){
			fseek(myhist,0,SEEK_SET);
			while(fscanf(myhist," %[^\n]",temp)!=EOF)printf("%s\n",temp);
		}
		else{
			i=atoi(args[0]+4);
			int no_of_lines=0;
			fseek(myhist,0,SEEK_SET);
			while(!fscanf(myhist," %*[^\n]"))no_of_lines++;
			fseek(myhist,0,SEEK_SET);
			while(no_of_lines>i){
				fscanf(myhist," %*[^\n]");
				no_of_lines--;
			}
			while(fscanf(myhist," %[^\n]",temp)!=EOF)printf("%s\n",temp);
		}
	}
        else if(!strncmp(args[0],"pid",3)){
            if(args[1]==NULL){
                printf("command name: %s process id: %d\n",elfname,getpid());
            }
            else{
//                for(i=start_pos+3;isspace(command[i]);i++);
                if(!strncmp(args[1],"all",3)){
                    printf("List of all process spawned by the shell\n");
                    for(i=0;i<proc_count;i++){
                        printf("command name: %s process id: %d\n",process[i].name,process[i].pid);
                    }
                }
                else if(!strncmp(args[1],"current",7)){
                    printf("List of currently executing processes spawned by the shell\n");
                    for(i=0;i<proc_count;i++){
                        if(process[i].is_running){
                           printf("command name: %s process id: %d\n",process[i].name,process[i].pid);
                        }
                    }
                }
            }
        }
	else{
        	child_id=fork();
		
	        if(child_id==-1){
	            printf("Fork failed. Command cannot be executed\n");
	        }
	
	        else if(child_id==0){
	            execvp((const char *)args[0],args);
	            perror("Command Not Found");
	            exit(1);
	        }
	
	        else{
			if(command[end_pos]=='|'){
				close(mypipe[1]);
				exec_command(end_pos+1,mypipe[0]);
			}
	            process[proc_count].pid=child_id;
	            process[proc_count].is_running=1;
	            for(i=start_pos;!isspace(command[i]) && i<end_pos;i++)process[proc_count].name[i]=command[i];
        	    proc_count++;
        	    if(!background_process){
        	        foreground_running=1;
               		waitpid(child_id,NULL,0);
            		foreground_running=0;
            		process[proc_count-1].is_running=0;
   	           }
        	   for(i=0;i<count;i++)free(args[count]);
		}
	}
	return;
}

void sigint_handler(int sig){
//	getc(stdin);
//	printf("\n");
//	print_promt();
}

int main(int argv,char *argc[]){
    strcpy(elfname,argc[0]);
    int i,j,n;
    char temp[1000],temp2[1000];
    b_stdout=dup(1);
    b_stdin=dup(0);
    b_stderr=dup(2);
    if(getcwd(home,100)==NULL){
            printf("Get current working directory method failed. Exiting\n");
            exit(1);
    }

    signal(SIGCHLD,sigchld_handler);
    signal(SIGINT,sigint_handler);
    signal(SIGTSTP,SIG_IGN);
    while(1){
    	int flag=1;
	myhist=fopen("hist.txt","a+");
	if(myhist==NULL)
		printf("History Log File Failed To Open, History will not be saved.\nAssociated commands might not work\n");
	while(1){
		memset(command,0,1000);
        	if(flag==1)print_promt();
		fgets(command,1000,stdin);
		flag=0;
		if(strlen(command)>0)flag=1;
        	for(i=strlen(command)-1;isspace(command[i])||command[i]=='\n';i--)command[i]='\0';
		for(i=0;i<strlen(command);i++)if(!isspace(command[i]))break;
		memmove(command,command+i,sizeof(char)*(strlen(command+i)+1));
		if(strlen(command)>0)break;
	}
	for(i=0;i<strlen(command);i++){
		if(command[i]=='!'){
			if(!strncmp(command+i,"!hist",5)){
				fseek(myhist,0,SEEK_SET);
				if(isdigit(command[i+5])){
					j=atoi(command+i+5);
					n=0;
					while(isdigit(command[i+n]))n++;
					if(j>0){
						j--;
						while(fscanf(myhist," %[^\n]",temp)!=EOF && j>0)j--;
					}
					if(j>0||j<0){
						printf("Command Not in History File\n");
						continue;
					}
					else{
						strcpy(temp2,command+i+6+n);
						strcpy(command+i,temp);
						strcat(command,temp2);		
					}
				}
			}
		}
	}
	fseek(myhist,0,SEEK_END);
	fputs(command,myhist);
	fputs("\n",myhist);
	fflush(myhist);

        if(!strncmp(command,"cd",2)){
            if(strlen(command)==2){
                if(chdir((const char *)home))
                    printf("cd failed to execute\n");
            }
            else{
		for(i=3;isspace(command[i]);i++);
                if(chdir((const char *)&command[i]))
                    printf("Invalid Path or argument to cd.\n");
            }
        }
        else if(!strcmp(command,"quit"))exit(0);
        else{
		exec_command(0,0);
		dup2(b_stdout,1);
		dup2(b_stdin,0);
		dup2(b_stderr,2);
        }
    }
    return 0;
}
