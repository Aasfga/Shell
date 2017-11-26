#include <stdio.h>
#include <shl_execute.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <shl_children.h>
#include <signal.h>
#include "config.h"
#include "shl_io.h"




//ma robić pipeline (jest więcej komand niz jedna)

#define FILE "/Users/Dominik/Documents/Programowanie/C/Studia/SO/Shell/suites/1/input/4.in"
#define DEBUG 1

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
	set_new_mask();
	signal(SIGCHLD, sigchld_handler);
}

int main(int argc, char *argv[])
{

	set_signals();
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

