#ifndef WISH_H
#define WISH_H

int exit_check(char *command[], int arg_count);
int path_check(char *command[], char *PATH[], int arg_count);
int cd_check(char *command[], int arg_count);
void error();

#endif /* WISH_H */
