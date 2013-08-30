//To add: pipes and redirection

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<pwd.h>
#include<errno.h>
#include<signal.h>


char OrigD[200];
size_t sizeD = 200;

char history[1000][100];
int his_count = 0;

typedef struct ss {
	char name[100];
	pid_t pid;
	int is_running;
} s_proc;

s_proc processes[100];

int count_proc = 0;

void headOut();
int maxmatch(char *s1, char *s2);
void fg_handler(int num);
void bg_handler(int num);
void other_handler(int num);
int newSet(char *buffer);
void exitme(int num){exit(0);}

int main(int argv, char *argc[]){
	int s=1;
	for(s=1;s<32;s++)
		signal(s, other_handler);
	signal(SIGCHLD, bg_handler);
	signal(SIGINT, exitme);
	char *buffer, *cmd, *args[100];
	int n_args=0;
	size_t buf_len=0;
	ssize_t buf_read;
	
	//printf("CWD: %s\n", getcwd(OrigD, 200));
	getcwd(OrigD, 200);


	/* -----Read Process-----
	 * buf_read = getline...
	 * buf_read contains the length of the string
	 * Decrement buf_read to compensate for \n, don't want it
	 * Use freely
	 * [Required?] Free it and change the value of buffer to NULL
	 * Apparently, freeing not required if being reused. Same pointer is reallocated additional space if required. Light hai. :D
	 * ----------------------
	 */

	int flag = 0;
	int cmd_type;
	while(1){

		//printf("CWD: %s\n", getcwd(OrigD, 200));

		n_args = 0; //Reinitialize the number of arguments

	
		if(!flag){
			headOut();
			buf_read = getline(&buffer, &buf_len, stdin);
			buf_read--;
			buffer[buf_read]='\0';
		}
		else
			flag = 0;

		//printf("DEBUG: %s\n", buffer);
	
		if(strcmp(buffer, "")){
			strcpy(history[his_count++], buffer);
			//history[his_count++] = buffer;
			//printf("%s\n", buffer);
		}
		if(strcmp(buffer, "")==0) continue;

		if(cmd_type=newSet(buffer)){
			char * cargs[100];
			int bg=0, varpipe = 1, n_cargs=0, i, next_is_input = 0, next_is_output = 0;
			char *out = NULL, *in = NULL;
			if(strchr(buffer, '&'))
				bg = 1;
			//------------------------------------------------------------------
			//Put all pipe-separated commands in args array
			//printf("Pipe: %d\n", varpipe);
			args[0]=strtok(buffer, "|");
			n_args++;
			while(args[n_args++]=strtok(NULL, "|"));
			n_args--;
			if(args[1]==NULL)
				varpipe = 0;
			//for(i=0;i<n_args;i++)
			//	printf("Cmd: %s\n", args[i]);
			//-------------------------------------------------------------------
			pid_t master_pid;
			int master_status;
			if((master_pid = fork()) == 0)
			{
				//Array args contains pipe-separated commands, goes from 0 to n_args-1
				//To do: run each command in a separate fork
				//The first command may take input from external file, and the last command may
				//push output to external file
				//Command can also be hist/pid, think about that too
				
				//All kids execution starts here
				int c;
				int status;
				int pids[n_args];
				int pid_c = 0;
				int pipefd[n_args][2];
				for(c=0;c<n_args;c++)//Each c runs a pipe separated command
				{
					if( pipe(pipefd[c]) < 0)
						perror("Piping failed");
					char *cmd = strtok(args[c], "\t ");//Main execution program
					//int pids[n_args];
					//int pid_c = 0;
					cargs[0]=cmd; 
					n_cargs = 0;
					n_cargs++;
					next_is_input = 0;
					next_is_output = 0;
					in = NULL, out = NULL;

					//If c = 0, input comes from stdin or in, output goes to pipefd[c][1]
					//If c = n-1, input comes from pipefd[c-1][1], output goes to stdout or out
					//Else input comes from pipefd[c-1][1] and output goes to pipefd[c][0]

					while(cargs[n_cargs++] = strtok(NULL, " \t")){
						i = n_cargs - 1;
						//printf("subcmd: %s\n", cargs[i]);
						if(!strncmp(cargs[i], "<", 1) || next_is_input){
							if(next_is_input){
								next_is_input = 0;
								in = cargs[i];
							}
							else if(strlen(cargs[i])>1){
								in = cargs[i]+1;
							}
							else{
								next_is_input = 1;
							}
							cargs[i] = NULL;


						}
						else if(!strncmp(cargs[i], ">", 1) || next_is_output){
							if(next_is_output){
								next_is_output = 0;
								out = cargs[i];
							}
							else if(strlen(cargs[i])>1){
								out = cargs[i]+1;
							}
							else{
								next_is_output = 1;
							}
							cargs[i] = NULL;
						}
					}
					i=0;

					//Debug section
					//while(cargs[i]!=NULL)
					//	printf("-->%s\n", cargs[i++]);
					//printf("-->%s\n", cargs[0]);
					//printf("In: %s\nOut: %s\n", in, out);

					pid_t fork_pid;
					int status;
					
					//If c = 0, input comes from stdin or in, output goes to pipefd[c][1]
					//If c = n-1, input comes from pipefd[c-1][0], output goes to stdout or out
					//Else input comes from pipefd[c-1][0] and output goes to pipefd[c][1]

					if((pids[pid_c++] = fork())==0){

						int fdin, fdout;
						if(in != NULL && c == 0){
							fdin = open(in, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
							dup2(fdin, STDIN_FILENO);
						}
						if(out != NULL && c == n_args-1){
							fdout = open(out, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
							dup2(fdout, STDOUT_FILENO);
						}
						if(c != 0){
							close(pipefd[c-1][1]);/*Close write end*/
							if(dup2(pipefd[c-1][0], 0) != STDIN_FILENO)
								perror("DUP ERROR!!!");
							//close(pipefd[c-1][0]);
						}
						//printf("%s finishedi %d\n", cmd, getpid());

						if(c != n_args-1){
							close(pipefd[c][0]);/*Close read end*/
							dup2(pipefd[c][1], STDOUT_FILENO);
							//close(pipefd[c][1]);
						}
						int du;
						for(du=0;du<c;du++)
						{
							close(pipefd[c][0]);
							close(pipefd[c][1]);
						}
						if(execvp(cmd, cargs)==-1)
							perror("Command not found");
						/*if(c != 0)
							close(pipefd[c-1][0]);
						if(c != n_args-1)
							close(pipefd[c][1]);
						printf("Done\n");*/
						/*if(in != NULL)
							close(fdin);
						if(out !=NULL)
							close(fdout);
						fflush(stdout);*/
						_exit(0);
					}
					else
					{
						int ij;
						if(c==n_args-1)
						{
							for(ij = 0; ij < n_args; ij++)
							{
								//printf("AJ: %d\n", pids[ij]);
								close(pipefd[c][0]);
								close(pipefd[c][1]);
							}
						//	waitpid(pids[0], NULL, 0);
						//	waitpid(pids[2], NULL, WNOHANG);
						//	waitpid(pids[0], NULL, 0);
						}
					}
					//All kids execution ends here
				}
				//for(i=0;i<n_args-1;i++);
				//printf("PID is %d\n", pids[0]);
				waitpid(pids[0], &status, 0);
				//printf("PID is %d\n", pids[0]);
				_exit(0);
			}
			else{
				//printf("Master wait\n");
				//waitpid(master_pid, NULL, 0);
			}

		}
		else{
			cmd = strtok(buffer, " \t");
			//printf("%s\n", cmd);
			//strcpy(args[0], cmd);
			args[0] = cmd;
			n_args++;

			while(args[n_args] = strtok(NULL, " \t")){
				//strcpy(args[n_args++], temp);
				n_args++;
				//printf("%s\n", args[n_args-1]);
			}
			args[n_args] = (char *)NULL;
			//printf("Number of args: %d\n", n_args);
			if(!strcmp(cmd, "exit") || !(strcmp(cmd, "quit"))){
				exit(0);
			}
			else if(!strcmp(cmd, "")){
				;
			}
			else if(!strcmp(cmd, "pid")){
				if(n_args==1){
					printf("command name: %s process id: %d\n", argc[0], getpid());
				}
				else if(!strcmp(args[1], "all")){
					printf("List of all processes spawned from this shell:\n");
					int i;
					for(i=0;i<count_proc;i++){
						printf("command name: %s process id: %d\n", processes[i].name, processes[i].pid);
					}
				}
				else if(!strcmp(args[1], "current")){
					printf("List of currently executing processes spawned from this shell:\n");
					int i;
					for(i=0;i<count_proc;i++){
						if(processes[i].is_running == 1)
							printf("command name: %s process id: %d\n", processes[i].name, processes[i].pid);
					}
				}

			}
			else if(!strcmp(cmd, "cd")){
				if(n_args == 1){
					//printf("Yo yo %s\n", OrigD);
					chdir((const char *)OrigD);
				}
				//printf("CD detected\n%s\n", args[0]);
				else if(chdir((const char *)args[1]))
					printf("Invalid arguments/permission denied\n");
			}
			else if(!strncmp(cmd, "hist", 4)){
				//printf("History commands\n");
				if(strlen(cmd)>4){
					char *hnum = cmd + 4;
					long int hint = strtol(hnum, NULL, 10);
					//printf("Hist N %ld\n", hint);
					int h, z=0;
					if (hint > his_count) {
						printf("You haven't typed this many commands yet.\n");
						continue;
					}
					for(h = his_count - hint - 1; h < his_count - 1; h++){
						printf("%d. %s \n", ++z, history[h]);
					}
				}
				//if it's hist
				else
				{
					int h;
					for(h=0;h<his_count-1;h++){
						printf("%d. %s\n", h+1, history[h]);
					}
				}
			}
			else if(!strncmp(cmd, "!hist", 5)){

				char *lnum = cmd + 5;
				long int lint = strtol(lnum, NULL, 10);
				if(lint > his_count){
					printf("You haven't typed this many commands yet.\n");
				}
				//printf("Command %s needs to be run\n", history[lint-1]);
				//buffer = history[lint-1];
				buffer = (char *)malloc(sizeof(char)*strlen(history[lint-1]));
				strcpy(buffer, history[lint-1]);
				//printf("%s\n", buffer);
				flag = 1;

			}
			else if(n_args == 2 && args[1][0] == '&'){//Condition for background process can be improved

				pid_t bg_pid;
				int status;
				pid_t result;
				char *bg_arg[2];
				bg_arg[0] = cmd;
				bg_arg[1] = NULL;

				if((bg_pid = fork()) == 0){
					//Execvp				
					if(execvp(cmd, bg_arg) == -1){
						printf("Command not found\n");
						_exit(1);
					}
					_exit(2);
					//printf("%s %s\n", bg_arg[0], bg_arg[1]);
				}
				else if(bg_pid == -1){
					printf("Unable to fork a background process.\n");
				}
				else
				{
					//Add to processes list
					printf("command %s pid %d\n", cmd, bg_pid);
					processes[count_proc].pid = bg_pid;
					strcpy(processes[count_proc].name, cmd);
					processes[count_proc].is_running = 1;
					count_proc++;
					//continue;
				}

			}
			else if(cmd[0] >= 'A' && cmd[0] <= 'z'){
				//printf("%s\n", cmd);
				//printf("%s\n", cmd);
				pid_t fork_pid;
				int status;
				pid_t result;

				if((fork_pid = fork()) == 0){			
					if(execvp(cmd, args)==-1){
						printf("Command not found.\n");
					}
					_exit(1);
				}
				else if(fork_pid == -1){
					printf("Fork failed!\n");
				}
				else {
					signal(SIGCHLD, fg_handler);
					result = wait(&status);
					signal(SIGCHLD, bg_handler);
					processes[count_proc].pid = fork_pid;
					strcpy(processes[count_proc].name, cmd);
					processes[count_proc].is_running = 0;
					count_proc++;
				}

			}
		}
			

	}

	return 0;
}
int newSet(char *buffer){//True if buffer contains / or <>
	if(strchr(buffer, '|'))
		return 2;
	if(strchr(buffer, '<') || strchr(buffer, '>'))
		return 1;	
	return 0;
}
//s1 is home folder, s2 is the other string
int maxmatch(char *s1, char *s2){

	int l1 = strlen(s1);
	int l2 = strlen(s2);
	int i, j;
	if(l2<l1)
	{
		//strcpy(s3, s2);
		return -1;
	}
	else{
		int flag = 1;
		for(i=0;i<l1;i++){
			if(s1[i]!=s2[i])
				break;
		}
		if(s2[i]=='/')
			return i;
		else
			return -1;
	}
}

void fg_handler(int num){
	//printf("Foreground handler\n");
	return;
}
void bg_handler(int num){
	pid_t bg_pid;
	int status, i;
	bg_pid = wait(&status);
	for(i=0;i<count_proc;i++){
		if(processes[i].pid == bg_pid){
			printf("\n%s %d ", processes[i].name, processes[i].pid);
			if(WIFEXITED(status)){
				printf("exited normally\n");
				headOut();
				processes[i].is_running = 0;
				fflush(stdout);
				return;
			}
			break;
		}
	}
}
void other_handler(int num){
	printf("Please type exit or quit to exit.\n");
	fflush(stdout);
	return;
}

void headOut(){

	struct passwd *passwd;
	char cwd[200], temp[200];
	int headflag, c=0, i;

	getcwd(cwd, 200);

	char host[100];
	size_t h_len = 100;
	if(!strcmp(cwd, OrigD)){
		strcpy(cwd, "~");
	}
	else if((headflag=maxmatch(OrigD, cwd))!=-1){
		temp[0]='~';
		for(i=headflag;i<strlen(cwd);i++){
			temp[++c]=cwd[i];
		}
		temp[++c]='\0';
		strcpy(cwd, temp);
	}
	gethostname(host, h_len);
	passwd = getpwuid(getuid());
	printf("<%s@%s:%s> ", passwd->pw_name, host, cwd);
}
