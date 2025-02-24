#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "wish.h"

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10
#define MAX_PATH_LENGTH 10

int main(int argc, char *argv[]) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGUMENTS];
    char *PATH[MAX_PATH_LENGTH];
    char *temp_path;     
    int cd,path;    //built in checks
    int i;
    bool batch = false;

    //set default path
    PATH[0] = "/bin";
    //set the rest of PATH to " ", with NULL terminator
    for(i = 1; i < MAX_PATH_LENGTH - 1; i++) {
        PATH[i] = " ";
    }
    PATH[MAX_PATH_LENGTH - 1] = '\0';
    
    if(argc == 2) {
        //batch mode
        freopen(argv[1], "r", stdin);
        batch = true;
    }

    //prompt/execution loop
    while(1) {
        if(!batch) {
            printf("wish> ");
        }
        fflush(stdout);

        //read command from user
        if(fgets(command, sizeof(command), stdin) == NULL) {
            printf("\n");
            break;
        }

        //remove trailing '\n'
        command[strcspn(command, "\n")] = '\0';
        
        //tokenize command into arguments
        char *token = strtok(command, " ");
        int arg_count = 0;
        while(token != NULL && arg_count < MAX_ARGUMENTS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        cd = 0;
        path = 0;

        //exit command check
        exit_check(args, arg_count);
        
        //cd check
        cd = cd_check(args, arg_count);
        
        //check path
        if(strcmp(args[0], "path") == 0) {
            //clear path
            for(i = 0; i < MAX_PATH_LENGTH; i++) {
                PATH[i] = " ";
            }

            //set path
            for(i = 1; i < arg_count; i++) {
                PATH[i - 1] = args[i];
                //printf("%s ", PATH[i - 1]);
            }
            path = 1;
        }

        //if one of the built ins ran, skip execution(not needed for exit)
        if(cd == 1 || path == 1) {
            continue;
        }

        //fork() a child process
        pid_t pid = fork();

        if(pid < 0) {
            fprintf(stderr, "An error has occurred\n");
        } else if (pid == 0) {
            i = 0;
            //checks all the paths
            while(strcmp(PATH[i], " ") != 0) {
                //concatenates the command to the path
                temp_path = malloc(strlen(PATH[i]) + strlen(args[0]) + 2);
                if (temp_path) {
                    sprintf(temp_path, "%s/%s", PATH[i], args[0]);
                    if(access(temp_path, X_OK) == 0) {
                        execv(temp_path, args);
                        fprintf(stderr, "An error has occurred");
                    }
                }
                i++;
            }
        } else {
            //parent process waits for the child to complete
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}

