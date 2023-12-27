#include <stdio.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 10000

char *cmd[MAX_INPUT_SIZE];
char *pipe_cmd[MAX_INPUT_SIZE];
int skip = 0, red = 0, red_index, pipe_flag = 0, pipe_index;

void split(char input[]){
    char* token = strtok(input, " "); 

    int cnt = 0;

    while(token != NULL){
        cmd[cnt] = token;
        if(strcmp(token, "&") == 0) {
            skip = 1;
            cmd[cnt] = NULL; 
        }
        if(strcmp(token, ">") == 0) { 
            red = 1;
            cmd[cnt] = NULL;
            red_index = cnt + 1; 
        }
        if(strcmp(token, "|") == 0) { 
            pipe_flag = 1; 
            cmd [cnt] = NULL; 
            pipe_index = cnt + 1; 
        }

        token = strtok(NULL, " "); 
        cnt++;
    }

    cmd[cnt] = NULL;
}

int main(){
    char input[MAX_INPUT_SIZE];

    while(1){

        printf("> ");
        fgets(input, MAX_INPUT_SIZE, stdin); 
        input[strlen(input)-1] = '\0';
        
        split(input);

        if(pipe_flag == 0) { 
            pid_t pid;

            pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork Failed");
                exit(-1);

            }
            else if (pid == 0) { 
 
                if(red == 1){
                    int fd = open(cmd[red_index], O_WRONLY | O_CREAT | O_TRUNC, 0666); 
                    dup2(fd, STDOUT_FILENO); 
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                
                execvp(cmd[0], cmd);
            }
            else{
                if(skip == 0) 
                    wait(NULL);
                else 
                    signal(SIGCHLD, SIG_IGN); 
            }
        }
        else {

            for(int i = 0; cmd[i + pipe_index] != NULL; i++) {
                pipe_cmd[i] = cmd[i + pipe_index];
                cmd[i + pipe_index] = NULL;
            }

            int pipe_fd[2]; 
            if(pipe(pipe_fd) == -1){
                perror("Pipe creation failed");
                exit(1);
            }
            pid_t left_pid, right_pid;
            left_pid = fork();
            if(left_pid == 0) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
                execvp(cmd[0], cmd);
                perror("Left command execution failed");
                exit(1);
            }

            right_pid = fork();
            if(right_pid == 0) {
                close(pipe_fd[1]);
                dup2(pipe_fd[0], STDIN_FILENO);
                close(pipe_fd[0]);
                execvp(pipe_cmd[0], pipe_cmd);
                perror("Right command execution failed");
                exit(1);
            }

            close(pipe_fd[0]);
            close(pipe_fd[1]);
            waitpid(left_pid, NULL, 0);
            waitpid(right_pid, NULL, 0);

        }

	    skip = red = red_index = pipe_flag = pipe_index = 0;
    }

    return 0;
}