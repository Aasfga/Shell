#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "builtins.h"

int echo(char *[]);

int undefined(char *[]);

builtin_pair builtins_table[] = {
		{"exit",  my_exit},
		{"lecho", &echo},
		{"lcd",   &undefined},
		{"lkill", &undefined},
		{"lls",   &undefined},
		{NULL, NULL}
};

int
echo(char *argv[])
{
	int i = 1;
	if(argv[i]) printf("%s", argv[i++]);
	while(argv[i])
		printf(" %s", argv[i++]);

	printf("\n");
	fflush(stdout);
	return 0;
}

int
undefined(char *argv[])
{
	fprintf(stderr, "Command %s undefined.\n", argv[0]);
	return BUILTIN_ERROR;
}


int my_exit(char **argv)
{
	exit(0);
}


int my_cd(char **argv)
{

}
