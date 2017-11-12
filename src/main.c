#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errors.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"
#include "siparse.h"
#include "utils.h"

#define DEBUG 0
#define FILE "/Users/Dominik/Documents/Programowanie/C/Studia/SO/Shell/Tests/suites/1/input/1.in"



int move_descriptor(int fd, int dest)
{
	int status = close(dest);
	if(status != 0)
		return -1;
	dup(fd);
	close(fd);
	return 0;
}

void print_error(int status)
{
	switch(status)
	{
		case ENOENT:
			write(2, NO_FILE, strlen(NO_FILE));
			break;
		case EACCES:
			write(2, NO_ACCESS, strlen(NO_ACCESS));
			break;
		default:
			write(2, EXEC_FAIL, strlen(EXEC_FAIL));
	}


}

void swap_stdin()
{
	close(0);
	int i = open(FILE, O_RDONLY, 0);
	i += 0;
}

void exec_error(char *name, int status)
{
	write(2, name, strlen(name));
	write(2, ": ", 2);
	print_error(status);
}

void print_prompt()
{

	struct stat b;
	fstat(0, &b);
	if(S_ISCHR(b.st_mode))
	{
		fflush(stdout);
		write(1, PROMPT_STR, strlen(PROMPT_STR));
		fflush(stdout);
	}
}

int find_end(const char *input, int i, int e)
{
	for(;i < e; i++)
	{
		if(input[i] == '\0' || input[i] == '\n')
			return i;
	}
	return -1;
}

int shl_read(char *input)
{
	static char buffer[BUFFER_SIZE];
	static int index = 0;
	static int start = 0;
	int end = find_end(buffer, start, index);
	int begin;
	int eof = 0;


	while(end == -1 && !eof)
	{
		ssize_t bytes = read(0, buffer + index, MAX_LINE_LENGTH);
		begin = index;
		index += bytes;

		eof = bytes == 0;

		if(index > 2 * MAX_LINE_LENGTH)
		{
			memmove(buffer, buffer + MAX_LINE_LENGTH, index - MAX_LINE_LENGTH);//poprawić zgodnie z pomysłem
			index -= MAX_LINE_LENGTH;
			start -= MAX_LINE_LENGTH;
			begin -= MAX_LINE_LENGTH;
		}

		end = find_end(buffer, begin, index);//ls ls
	}

	if(eof)
	{
		if(index - start > MAX_LINE_LENGTH)
		{
			start = index;
			return -1;
		}
		if(index - start > 0)
		{
			memcpy(input, buffer + start, end - start);
			input[index - start] = '\n';
			input[index - start + 1] = '\0';
			start = index;
		}
		else
		{
			input[0] = 0;
		}
	}
	else
	{
		if(end - start > MAX_LINE_LENGTH)
		{
			start = end + 1;
			return -1;
		}
		else if(end == start)
		{
			input[0] = '\n';
			input[1] = '\0';
			start++;
		}
		else
		{
			memcpy(input, buffer + start, end - start);
			input[end - start] = '\n';
			input[end - start + 1] = '\0';

			start = end + 1;
		}

	}

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

//ma robić pipeline (jest więcej komand niz jedna)

int shl_exec(command *com)
{
	set_redirections(com->redirs);
	execvp(com->argv[0], com->argv);
	int err = errno;
	exec_error(com ->argv[0], err);
	return EXEC_FAILURE;
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


			shl_exec(commands[i]);
		}
	}

	return 0;
}

//nie patrze na builtiny w przypadku poleceń wbudowanych
//budowa lini (średniki)
//jedna komenda
int shl_exec_command(command *com)
{
	if(!fork())
	{
		//if is builtin
		//use it
		//else
		//osobna funkcja
		exit(shl_exec(com));
	}

	int err = 0;
	wait(&err);


	if(WIFEXITED(err))
		err = WEXITSTATUS(err);
	else
		err = 0;

	return err;
}

void print_syntax_error()
{
	write(2, SYNTAX_ERROR_STR, strlen(SYNTAX_ERROR_STR));
	write(2, "\n", 1);
}

int shl_parseline(char *i, line **l)
{
	*l = parseline(i);

	if(*l == NULL)
		return -1;
	return 0;
}

int main(int argc, char *argv[])
{
	if(DEBUG)
		swap_stdin();

	char input[MAX_LINE_LENGTH + 1];
	line *l;
	command *c;
	int status;

	while(1)
	{
		print_prompt();

		status = shl_read(input);
		if(status == -1)
		{
			print_syntax_error();
			continue;
		}
		else if(!input[0])
			break;

		status = shl_parseline(input, &l);
		if(status == -1)
			continue;
		c = pickfirstcommand(l);
		if(c->argv[0] == NULL)
			continue;

		status = shl_exec_command(c);
		status = 0;
		fflush(stderr);

	}


	return 0;
}

