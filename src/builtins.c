#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>

#include "builtins.h"

int echo(char *[]);

int undefined(char *[]);

builtin_pair builtins_table[] = {
		{"exit",  my_exit},
		{"lecho", &echo},
		{"lcd",   my_cd},
		{"lkill", my_kill},
		{"lls",   my_ls},
		{"cd", my_cd},
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
	if(argv[1] != NULL && argv[2] != NULL)
		return -1;
	char *path = argv[1];
	if(argv[1] == NULL)
		path = getenv("HOME");
	if(path[0] == '~')
	{
		chdir(getenv("HOME"));
		path += 2;
	}

	if(path[0] != '\0')
		return chdir(path);
	return 0;
}


int my_kill(char **argv)
{
	if(argv[1] == NULL)
		return -1;
	if(argv[2] != NULL && argv[3] != NULL)
		return -1;

	int pid;
	int sig;
	if(argv[2] == NULL)
	{
		pid = atoi(argv[1]);
		sig = SIGTERM;
	}
	else
	{
		pid = atoi(argv[2]);
		sig = atoi(argv[1] + 1);
	}
	return kill(pid, sig);
}

int my_ls(char **argv)
{

	char path[1024] = {0};
	if(argv[1] == NULL)
	{
		if(getcwd(path, 1024) == NULL)
			return -1;
//		memcpy(path, getenv("PWD"), strlen(getenv("PWD")));
	}
	else if(argv[2] != NULL)
		return -1;
	else if(argv[1][0] == '~')
	{
		char *home = getenv("HOME");
		memcpy(path, home, strlen(home));
		memcpy(path + strlen(home), argv[1] + 1, strlen(argv[1] + 1));
	}
	else
		memcpy(path, argv[1], strlen(argv[1]));

	DIR *dp = opendir(path);
	struct dirent *ep;

	if (dp != NULL)
	{
		for(ep = readdir(dp); ep != NULL; ep = readdir(dp))
		{
			if(ep->d_name[0] == '.')
				continue;
			else
			{
				write(1, ep->d_name, strlen(ep->d_name));
				write(1, "\n", 1);
			}
		}

		closedir (dp);
	}
	else
		return -1;
	return 0;
}