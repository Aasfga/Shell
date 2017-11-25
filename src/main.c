#include <stdio.h>
#include <shl_execute.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <shl_children.h>
#include "config.h"
#include "shl_io.h"
#include <signal.h>



//ma robić pipeline (jest więcej komand niz jedna)

#define FILE "/Users/Dominik/Documents/Programowanie/C/Studia/SO/Shell/0.in"
#define DEBUG 0

void swap_stdin()
{
	close(0);
	int i = open(FILE, O_RDONLY, 0);
	i += 0;
}


int shl_parseline(char *i, line **l)
{
	*l = parseline(i);

	if(*l == NULL)
		return -1;
	return 0;
}

void set_signals()
{
	sigprocmask(SIG_BLOCK, NULL, &default_mask);
	signal(SIGINT, int_handler);
	signal(SIGCHLD, child_handler);
	block_sigchld();
}

int main(int argc, char *argv[])
{
	set_signals();
	setsid();
	if(DEBUG)
		swap_stdin();

	char input[MAX_LINE_LENGTH + 1];
	line *l;
	command *c;

	while(1)
	{
		print_prompt();

		if(shl_read(input) == -1)
		{
			print_error(128);
			continue;
		}
		else if(!input[0])
			break;
		else if(input[0] == '\n')
			continue;

		if(shl_parseline(input, &l) == -1)
			continue;


		if(shl_exec(l) < 0)
		{
			printf("EXEC_ERROR!\n");
			break;
		}
	}


	return 0;
}

