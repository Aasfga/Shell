//
// Created by Dominik Grybos on 25.11.2017.
//

#ifndef SHELL_SHL_CHILDREN_H
#define SHELL_SHL_CHILDREN_H

#include <signal.h>

sigset_t default_mask;
int fg_list[400];
void child_handler(int sig);
int add_fg(int pid);
int remove_fg(int pid);
int is_fg(int pid);
int fg_size();
void block_sigchld();
void unblock_sigchld();

#endif //SHELL_SHL_CHILDREN_H
