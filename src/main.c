#include <stdio.h>
#include <shl_execute.h>
#include "config.h"
#include "shl_io.h"




//ma robić pipeline (jest więcej komand niz jedna)





//nie patrze na builtiny w przypadku poleceń wbudowanych
//budowa lini (średniki)
//jedna komenda


int shl_parseline(char *i, line **l)
{
	*l = parseline(i);

	if(*l == NULL)
		return -1;
	return 0;
}

int main(int argc, char *argv[])
{
	char input[MAX_LINE_LENGTH + 1];
	line *l;
	command *c;
	int status;

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
			continue;
	}


	return 0;
}

