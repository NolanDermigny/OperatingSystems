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
    char *path_element;
    int cdC,pathC,exitC;    //built in checks
    int i;
    bool batch = false;
    //set default path
    PATH[0] = "/bin";
    
    if(argc == 2) {
        //batch mode
        freopen(argv[1], "r", stdin);
        batch = true;
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
        
 
        //tokenize command into arguments
        char *token = strtok(command, " ");
        int arg_count = 0;
        while(token != NULL && arg_count < MAX_ARGUMENTS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

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
                //conacatenate "/" to front of args as per test 7
                path_element = malloc(strlen(args[i]) + 1);
                sprintf(path_element, "/%s", args[i]);
                PATH[i - 1] = path_element;
                printf("%s\n, ", PATH[i - 1]);
            }
            pathC = 1;
        }

        //if one of the built ins ran, skip execution(not needed for exit)
        if(cdC == 1 || pathC == 1 || exitC == 1) {
            continue;
        }

        //fork() a child process
        pid_t pid = fork();

        if(pid < 0) {
            error();
        } else if (pid == 0) {
            i = 0;
            bool run = false;
            //checks all the paths
            while(PATH[i] != NULL) {
                //concatenates the command to the path
                temp_path = malloc(strlen(PATH[i]) + strlen(args[0]) + 2);
                if (temp_path) {
                    sprintf(temp_path, "%s/%s", PATH[i], args[0]);
                    printf("%s\n", temp_path);
                    if(access(temp_path, X_OK) == 0) {
                        run = true;
                        printf("%s being executed", temp_path);
                        execv(temp_path, args);
                        printf("throwing an execv error\n");
                        error();
                    }
                }
                if(run == true) {
                    break;
                }
                free(temp_path);
                i++;
            }
            //no valid path found
            printf("no path found error\n");
            error();
            exit(0);
        } else {
            //parent process waits for the child to complete
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}

