//
// Created by Dominik Grybos on 25.11.2017.
//

#include "shl_children.h"
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <shl_io.h>

#define FG_SIZE 400
#define EXIT_SIZE 400
int _bg_exits[EXIT_SIZE][2];
int iter = 0;
int fg_list[FG_SIZE] = {0};
int _fg_size = 0;

void add_status(int pid, int status)
{
	_bg_exits[iter][0] = pid;
	_bg_exits[iter][1] = status;
	iter = (iter + 1) % EXIT_SIZE;
}

void bg_exits()
{
	for(int i = 0; i < iter; i++)
	{
		print_status(_bg_exits[i][0], _bg_exits[i][1]);
	}
	iter = 0;
}

int add_fg(int pid)
{
	int i = 0;

	for(; i < FG_SIZE && fg_list[i] != 0; i++)
	{}

	if(i == FG_SIZE)
		return -1;
	fg_list[i] = pid;
	_fg_size++;
	return 0;
}

int remove_fg(int pid)
{
	int i = 0;

	for(; i < FG_SIZE && fg_list[i] != pid; i++)
	{}

	if(i == FG_SIZE)
		return -1;
	fg_list[i] = 0;
	_fg_size--;
	return 0;
}

int is_fg(int pid)
{
	int i = 0;
	for(; i < FG_SIZE && fg_list[i] != pid; i++)
	{}

	if(i == FG_SIZE)
		return 0;
	else
		return 1;
}

int fg_size()
{
	return _fg_size;
}

void child_handler(int sig)
{
	int status;
	int pid = waitpid(-1, &status, WNOHANG);
	while(pid > 0)
	{
		if(is_fg(pid))
			remove_fg(pid);
		else
			add_status(pid, status);
		pid = waitpid(-1, &status, WNOHANG);
	}

	signal(SIGCHLD, child_handler);
}

void int_handler(int sig)
{
	static int sig_counter = 0;
	sig_counter++;
	if(sig_counter == 2)
		sig_counter = 0;
	else
		killpg(0, SIGINT);
}

void block_sigchld()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, NULL);
}

void unblock_sigchld()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}