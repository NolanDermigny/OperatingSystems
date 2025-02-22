#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish.h"

#define MAX_PATH_LENGTH 10

//if command is exit, it exits
//throws an error if exit has more than 1 argument
int exit_check(char *command[], int arg_count) {
    if(strcmp(command[0],"exit") == 0 && arg_count == 1) {
        exit(EXIT_FAILURE);
    } else if(strcmp(command[0],"exit") == 0) {
        perror("Error: too many args in \"exit\"");
    }
    return 0;
}

/*
returns 1 if the path was changed and 0 if not`
*/
int path_check(char *command[], char *PATH[]) {
    if(strcmp(command[0],"path") == 0) {
        int i = 0;
        while(*PATH[i] != '\0') {
            PATH[i] = '\0';
            i++;
        }
        i = 1;
        while(*command[i] != '\0') {
            PATH[i - 1] = command[i];
        }
        return 1; 
    }
    return 0;
}

/*
returns 1 if cd executed, 0 otherwise
throws errors for too many args or invalid directory
*/
int cd_check(char *command[], int arg_count) {

    if(strcmp(command[0],"cd") == 0) {
        if(arg_count == 2) {
            if(chdir(command[1]) != 0) {
                perror("not a valid directory");
            }
        } else {
            int args = arg_count - 1;
            perror("invalid number of arguments");
        }
        return 1;   
    } else {
        return 0;
    }
}
 

        


            


