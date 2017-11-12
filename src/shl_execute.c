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

int move_descriptor(int fd, int dest)
{
	int status = close(dest);
	if(status != 0)
		return -1;
	dup(fd);
	close(fd);
	return 0;
}
int set_redirections(redirection **redirs)
{
	for(int i = 0; ; i++)
	{
		if(redirs[i] == NULL)
			break;
		int fd;
		int status;
		if(IS_RIN(redirs[i]->flags))
		{
			fd = open(redirs[i]->filename, O_RDONLY);
			if(fd < 0)
				return -1;
			status = move_descriptor(fd, 0);
			if(status < 0)
				return status;
		}
		else if(IS_ROUT(redirs[i]->flags))
		{
			fd = open(redirs[i]->filename, O_TRUNC | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
			if(fd < 0)
				return -1;
			status = move_descriptor(fd, 1);
			if(status < 0)
				return status;
		}
		else
		{
			fd = open(redirs[i]->filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if(fd < 0)
				return -1;
			status = move_descriptor(fd, 1);
			if(status < 0)
				return status;
		}

	}
	return 0;
}

int set_new_process(command *com)
{
	set_redirections(com->redirs);
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
	int id = 0;
	int fd[2];

	for(int i = 0; commands[i] != NULL; i++)
	{
		int status = pipe(fd);
		//zamykanie pipa jezeli nie stdin
		if(status < 0)
			return -1;
		if(!fork())
		{
			status = move_descriptor(id, 0);
			status = move_descriptor(fd[0], 1);
			if(status < 0)
				return -1;
			close(fd[1]);


			set_new_process(commands[i]);
		}
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
			return shl_exec_command(p[0]);
		}

	}
	return 0;
}