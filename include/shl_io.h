//
// Created by Dominik Grybos on 12.11.2017.
//

#ifndef SHELL_READER_H
#define SHELL_READER_H

#define NO_FILE "no such file or directory\n"
#define NO_ACCESS "permission denied\n"
#define EXEC_FAIL "exec error\n"
#define SYNTAX_ERROR 128



void print_prompt();

int shl_read(char *input);

void exec_error(char *name, int status);

void print_error(int status);

void builtin_error(char *name);

#endif //SHELL_READER_H
