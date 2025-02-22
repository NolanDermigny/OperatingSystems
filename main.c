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
    int cd,path;

    //set default path
    PATH[1] = "/bin";
    
    if(argc == 2) {
        //batch mode
        freopen(argv[1], "r", stdin);
    }

    while(1) {
        printf("wish> ");
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
        path = path_check(args, PATH);

        //if one of the built ins ran, skip execution(not needed for exit)
        if(cd == 1 || path == 1) {
            continue;
        }

        //fork() a child process
        pid_t pid = fork();

        if(pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            //child process executes the command
            execvp(args[0], args);
            //this should not return
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else {
            //parent process waits for the child to complete
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}

