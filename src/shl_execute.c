//
// Created by Dominik Grybos on 12.11.2017.
//

#include <stdlib.h>
#include <unistd.h>
#include <shl_io.h>
#include <errno.h>
#include <config.h>
#include <fcntl.h>
#include <stdlib.h>
#include <builtins.h>
#include <string.h>
#include "shl_execute.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>



int move_descriptor(int src, int dest)
{
	if(src == dest)
		return 0;
	if(dup2(src, dest) < 0)
		return -1;
	if(close(src) < 0)
		return -1;
	return 0;
}

int set_redirs(redirection **redirs)
{
	for(int i = 0; redirs[i] != NULL; i++)
	{
		int flag = 0;
		int dest = 1;
		if(IS_RIN(redirs[i]->flags))
		{
			flag = O_RDONLY;
			dest = 0;
		}
		else if(IS_ROUT(redirs[i]->flags))
		{
			flag = O_WRONLY | O_TRUNC | O_CREAT;
		}
		else if(IS_RAPPEND(redirs[i]->flags))
		{
			flag = O_WRONLY | O_APPEND | O_CREAT;
		}
		else
		{
			errno = 128;
			return -1;
		}

		int src = open(redirs[i]->filename, flag, S_IRUSR | S_IWUSR);
		if(src < 0)
			return -1;

		if(move_descriptor(src, dest) < 0)
		{
			return -1;
		}
	}
	return 0;
}

int set_new_process(command *com)
{
	if(set_redirs(com->redirs) < 0)
		exit(EXEC_FAILURE);
	execvp(com->argv[0], com->argv);
	int err = errno;
	exec_error(com ->argv[0], err);
	exit(EXEC_FAILURE);
}

int shl_exec_command(command *com)
{
	if(!fork())
	{
		exit(set_new_process(com));
	}

	int err = 0;
	wait(&err);

	if(WIFEXITED(err))
		err = WEXITSTATUS(err);
	else
		err = 0;

	return err;
}


int is_builtin(command *com)
{
	for(int i = 0; builtins_table[i].fun != NULL; i++)
	{
		if(strcmp(builtins_table[i].name, com->argv[0]) == 0)
			return i;
	}

	return -1;
}

int shl_exec(line *line)
{
	pipeline p;

	for(int i = 0; line->pipelines[i] != NULL; i++)
	{
		p = line->pipelines[i];

		if(p[0]->argv[0] == NULL)
			continue;
		int b = is_builtin(p[0]);
		if(b >= 0)
		{
			if(builtins_table[b].fun(p[0]->argv) != 0)
				builtin_error(builtins_table[b].name);
			else
				return 0;
		}
		else
		{
			if(shl_exec_command(p[0]) < 0)
				return -1;
		}

	}
	return 0;
}