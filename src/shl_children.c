//
// Created by Dominik Grybos on 26.11.2017.
//

#include <sys/wait.h>
#include <signal.h>
#include <shl_io.h>
#include <stdio.h>
#include "shl_children.h"
#include "stdlib.h"

#define SIZE 400

int _exits[SIZE][2] = {0};
int iter = 0;
sigset_t _mask;
int _fg_list[SIZE] = {0};
int _size = 0;

void set_new_mask()
{
	sigset_t new_mask;
	sigemptyset(&new_mask);
	sigaddset(&new_mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &new_mask, &_mask);
}

sigset_t give_mask()
{
	return _mask;
}

void add_exit(int pid, int status)
{
	_exits[iter][0] = pid;
	_exits[iter][1] = status;
	iter = (iter + 1)%SIZE ;
}

int add_fg(int pid)
{
	int i;
	for(i = 0; i < SIZE && _fg_list[i] != 0; i++)
	{}

	if(i == SIZE)
		return -1;
	_size++;
	_fg_list[i] = pid;
	return 0;
}

void remove_fg(int pid)
{
	int i;
	for(i = 0; i < SIZE && _fg_list[i] != pid; i++)
	{}

	if(i < SIZE)
	{
		_fg_list[i] = 0;
		_size--;
	}
}

int is_fg(int pid)
{
	int i;
	for(i = 0; i < SIZE && _fg_list[i] != pid; i++)
	{}

	if(i < SIZE)
		return 1;
	return 0;
}

int fg_size()
{
	return _size;
}

void sigchld_handler(int sig)
{
	int status;
	int pid = waitpid(-1, &status, WNOHANG);

	while(pid > 0)
	{
		if(is_fg(pid))
			remove_fg(pid);
		else
			add_exit(pid, status);
		pid = waitpid(-1, &status, WNOHANG);
	}


	signal(SIGCHLD, sigchld_handler);
}

void sigint_handler(int sig)
{
	for(int i = 0; i < SIZE; i++)
	{
		if(_fg_list[i] != 0)
		{
			kill(_fg_list[i], SIGKILL);
		}
	}
}

void print_exits()
{
	for(int i = 0; i  < iter; i++)
	{
		print_status(_exits[i][0], _exits[i][1]);
	}
	fflush(stdout);
	iter = 0;
}

void unblock_sigchld()
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void block_sigchld()
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, NULL);
}
