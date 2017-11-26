//
// Created by Dominik Grybos on 26.11.2017.
//

#ifndef SHELL_SHL_CHILDREN_H
#define SHELL_SHL_CHILDREN_H

void set_new_mask();

sigset_t give_mask();

int add_fg(int pid);

void remove_fg(int pid);

int is_fg(int pid);

int fg_size();

void sigchld_handler(int sig);

void sigint_handler(int sig);

void print_exits();

void unblock_sigchld();

void block_sigchld();

#endif //SHELL_SHL_CHILDREN_H
