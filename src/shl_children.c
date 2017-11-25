//
// Created by Dominik Grybos on 25.11.2017.
//

#include "shl_children.h"
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

#define FG_SIZE 400


int fg_list[FG_SIZE] = {0};
int _fg_size = 0;

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
	int pid = 1;
	int status = -1;
	while(pid > 0)
	{
		if(is_fg(pid))
			remove_fg(pid);
//		else
			//dodaj do zakończonych
		pid = waitpid(-1, &status, WNOHANG);
	}

	signal(SIGCHLD, child_handler);
}

void block_sigchld()
{
	sigset_t set;
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, NULL);
}

void unblock_sigchld()
{
	sigset_t set;
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}