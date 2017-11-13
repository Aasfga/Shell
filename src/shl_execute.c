//
// Created by Dominik Grybos on 12.11.2017.
//

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

int move_descriptor(int fd, int dest)
{
	if(fd == dest)
		return 0;
	if(close(dest) < 0)
		return -1;
	dup(fd);
	close(fd);
	return 0;
}

int set_redir(int dest_fd, char *filename, int flags, ...)
{
	va_list list;
	va_start(list, 1);
	int fd = open(filename, flags, va_arg(list, int));
	if(fd < 0)
		return -1;
	return move_descriptor(fd, dest_fd);
}

int set_redirs(redirection **redirs)
{
	int fd;
	int status;
	for(int i = 0; redirs[i] != NULL; i++)
	{
		if(IS_RIN(redirs[i]->flags))
		{
			if(set_redir(0, redirs[i]->filename, O_RDONLY) < 0)
				return -1;
		}
		else if(IS_ROUT(redirs[i]->flags))
		{
			if(set_redir(1, redirs[i]->filename, O_TRUNC | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR) < 0)
				return -1;

		}
		else
		{
			if(set_redir(1, redirs[i]->filename, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR) < 0)
				return -1;
		}

	}
	return 0;
}


int set_new_process(command *com)
{
//	write(1, com->argv[0], strlen(com->argv[0]));
	//set_redirs(com->redirs);
	execvp(com->argv[0], com->argv);
	int err = errno;
	exec_error(com ->argv[0], err);
	return EXEC_FAILURE;
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

int shl_exec_pipeline(pipeline commands)
{
	int last = 0;
	int fd[2];
	int i;
	for(i = 0; commands[i] != NULL; i++)
	{
		//zamykanie pipa jezeli nie stdin
		if(commands[i + 1] == NULL)
		{
			fd[0] = -1;
			fd[1] = 1;
		}
		else if(pipe(fd) < 0)
			return -1;

		if(!fork())
		{
			if(move_descriptor(last, 0) < 0)
				exit(-1);
			if(move_descriptor(fd[1], 1) < 0)
				exit(-1);

			if(fd[0] > 0)
				close(fd[0]);

			set_new_process(commands[i]);
		}
		if(last > 1)
			close(last);
		last = fd[0];
		if(fd[1] > 1)
			close(fd[1]);
	}

	while(i > 0)
	{
		wait(NULL);
		i--;
	}
	return 0;
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
			return shl_exec_pipeline(p);
		}

	}
	return 0;
}