#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish.h"

#define MAX_PATH_LENGTH 10

//if command is exit, it exits
//throws an error if exit has more than 1 argument
int exit_check(char *command[], int arg_count) {
    if (command == NULL || command[0] == NULL) {
        return 0;
    }

    if(strcmp(command[0],"exit") == 0 && arg_count == 1) {
        exit(0);
    } else if(strcmp(command[0],"exit") == 0) {
        error();    
    }
    return 0;
}

/*NOT IN USE
returns 1 if the path was changed and 0 if not`
*/
int path_check(char *command[], char *PATH[], int arg_count) {
    if(strcmp(command[0],"path") == 0) {
        int i = 0;
        while(PATH[i] != NULL && *PATH[i] != '\0') {
            PATH[i] = " ";
            i++;
        }
        i = 1;
        while(i < arg_count) {
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
    
    if (command == NULL || command[0] == NULL) {
        error();
        return 0;
    }

    if(strcmp(command[0],"cd") == 0) {
        if (arg_count < 2 || command[1] == NULL) {
            error();
            return 1;
        }
        if(chdir(command[1]) != 0) {
            error();
            return 1;
        }
           
    } else {
        return 0;
    }
}

void error() {
    fprintf(stderr, "An error has occurred\n");
}
 

        


            


