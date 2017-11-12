//
// Created by Dominik Grybos on 16.10.2017.
//

#ifndef SHELL_ERRORS_H
#define SHELL_ERRORS_H

#define NO_FILE "no such file or directory\n"
#define NO_ACCESS "permission denied\n"
#define EXEC_FAIL "exec error\n"
#define SYNTAX_ERROR 128

void exec_error(char *name, int status);

void print_error(int status);


#endif //SHELL_ERRORS_H
