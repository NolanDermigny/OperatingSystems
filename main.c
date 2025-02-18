#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "wish.h"

int main(int argc, char *argv[]) {

    int line = 1;
    char buff[100];    //line buffer
    int limit = 100;     //buffer limit
    bool batch = 0;     //boolean for if in batch mode

    if(argc == 2) {

        //batch mode
        FILE *file = fopen(argv[1], "r");

        //loops through the batch file printing out each line
        while(feof(file) == 0) {
            fgets(buff, limit, file);
            if(feof(file)) {
                break;
            }
            printf("line %d: %s\n", line, buff);
            line++;
        }
    } else if(argc == 1) {
        //interatcive mode
        buff[0] = ' ';
        //continuously prompts user and echos the input
        while(strcmp(buff, "exit") != 0) {
            printf("wish> ");
            fgets(buff, limit, stdin);
            printf("line %d: %s\n", line, buff);
            line++;
        }

    } else {
        //error
        printf("error: too many arguments\n");
        exit(0);
    }

}

