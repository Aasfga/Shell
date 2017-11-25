//
// Created by Dominik Grybos on 12.11.2017.
//

#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <shl_children.h>
#include <sys/wait.h>
#include "shl_io.h"

#define DEBUG 1

int find_end(const char *input, int i, int e)
{
	for(;i < e; i++)
	{
		if(input[i] == '\0' || input[i] == '\n')
			return i;
	}
	return -1;
}

void print_status(int pid, int status)
{
	if(WIFEXITED(status))
	{
		status = WEXITSTATUS(status);
		printf("Background process %i terminated. (exited with status %i)\n", pid, status);
	}
	else
	{
		status = WSTOPSIG(status);
		printf("Background process %i terminated. (killed by signal %i)\n", pid, status);
	}
}

int shl_read(char *input)
{
	static char buffer[BUFFER_SIZE];
	static int index = 0;
	static int start = 0;
	int end = find_end(buffer, start, index);
	int begin;
	int eof = 0;


	unblock_sigchld();
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
			memcpy(input, buffer + start, index - start);
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
	block_sigchld();
	return 0;
}

void exec_error(char *name, int status)
{
	write(2, name, strlen(name));
	write(2, ": ", 2);
	print_error(status);
	fflush(stderr);
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
		case SYNTAX_ERROR:
			write(2, SYNTAX_ERROR_STR, strlen(SYNTAX_ERROR_STR));
			break;
		default:
			write(2, EXEC_FAIL, strlen(EXEC_FAIL));
	}
	fflush(stderr);
}

void builtin_error(char *name)
{
	write(2, "Builtin ", 8);
	write(2, name, strlen(name));
	write(2, " error.\n", 8);
	fflush(stderr);
}

void print_prompt()
{
	struct stat b;
	fstat(0, &b);
	if(DEBUG || S_ISCHR(b.st_mode))
	{
		bg_exits();
		write(1, PROMPT_STR, strlen(PROMPT_STR));
		fflush(stdout);
	}
}
