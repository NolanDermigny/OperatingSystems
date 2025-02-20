#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "wish.h"

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10

int main(int argc, char *argv[]) {
	char command[MAX_COMMAND_LENGTH];
	char *args[MAX_ARGUMENTS];
	
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

