#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "wish.h"

#define MAX_COMMAND_LENGTH 200
#define MAX_ARGUMENTS 10
#define MAX_PATH_LENGTH 10

int main(int argc, char *argv[]) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGUMENTS];
    char *PATH[MAX_PATH_LENGTH];
    char *temp_path; 
    char *path_element;
    int cdC, pathC, exitC;    //built in checks
    int i;
    bool batch = false;     //batch mode flag
    bool redirect = false;  //redirection flag

    for(i = 0; i < MAX_PATH_LENGTH; i++) {
        PATH[i] = NULL;
    }
    //set default path
    PATH[0] = "/bin";
    
    if(argc == 2) {
        //batch mode
        if(freopen(argv[1], "r", stdin) == NULL){
            error();
            exit(1);
        }
        batch = true;
    } else if( argc > 2) {
        // to many args
        error();
        exit(1);
    }

    //prompt/execution loop
    while(1) {
        if(batch == false) {
            printf("wish> ");
        }
        fflush(stdout);

        //read command from user
        if(fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        //remove trailing '\n'
        command[strcspn(command, "\n")] = '\0';
        
        //removing whitespace
        if(command[0] == ' ') {
            char *cmd_ptr = &command[0];
            while(*cmd_ptr == ' ') {
                cmd_ptr++;
            }
            strcpy(command, cmd_ptr);
        }
        
        char *outfile = NULL;
        bool redirect = false;

        //Redirect Check
        char *redirect_pos = strchr(command, '>');
        if (redirect_pos != NULL) {
            redirect = true;
            *redirect_pos = '\0';
            redirect_pos++; 

            while (*redirect_pos == ' ') {
                redirect_pos++;
            }
            //no file after >
            if (*redirect_pos != '\0') {
                outfile = redirect_pos;
            } else {
                error();
                continue;
            }
            // greater than 1 > symbol
            if(strchr(outfile, '>') != NULL) {
                error();
                continue;
            }
            //greater than 1 file
            if(strchr(outfile, ' ') != NULL) {
                error();
                continue;
            } 
                
        }        
        
        // Now tokenize the command part into args
        int arg_count = 0;
        char *token = strtok(command, " ");
        while (token != NULL && arg_count < MAX_ARGUMENTS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
      
        args[arg_count] = NULL;

        //zero arg check, can happen from redirection
        if(arg_count == 0) {
            error();
            continue;
        }


        cdC = 0;
        pathC = 0;
        exitC = 0;

        //exit command check
        exitC = exit_check(args, arg_count);
        
        //cd check
        cdC = cd_check(args, arg_count);
        
        //check path
        if(strcmp(args[0], "path") == 0) {
            //clear path
            for(i = 0; i < MAX_PATH_LENGTH; i++) {
                PATH[i] = NULL;
            }

            //set path
            for(i = 1; i < arg_count; i++) {
                //need to allocate correctly for PATH
                path_element = malloc(strlen(args[i]) + 1); 
                sprintf(path_element, "%s", args[i]);
                PATH[i - 1] = path_element;
            }
            pathC = 1;
        }

        //if one of the built ins ran, skip execution
        if(cdC == 1 || pathC == 1 || exitC == 1) {
            continue;
        }

        //fork() a child process
        pid_t pid = fork();

        if(pid < 0) {
            error();
        } else if (pid == 0) {
            //redirect if needed
            if(redirect == true) {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);                
                if (fd < 0) {
                    exit(0);
                }   
                printf("fd = %d\n", fd);
                // Redirect stdout to the file
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    exit(0);
                }   

                close(fd);
            }

            i = 0;
            bool run = false;
            //checks all the paths
            while(PATH[i] != NULL) {
                //concatenates the command to the path
                temp_path = malloc(strlen(PATH[i]) + strlen(args[0]) + 2);
                if (temp_path) {
                    sprintf(temp_path, "%s/%s", PATH[i], args[0]);
                    if(access(temp_path, X_OK) == 0) {
                        run = true;
                        execv(temp_path, args);
                        free(temp_path);
                        exit(0);
                        
                    }
                    free(temp_path);
                }
                if(run == true) {
                    break;
                }
                i++;
            }
            //no valid path found
            if(run == false) {
                error();
                //printf("no valid path found error \n");
                exit(0);
            }
        } else {
            //parent process waits for the child to complete
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}
