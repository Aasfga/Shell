//
// Created by Dominik Grybos on 12.11.2017.
//

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <config.h>
#include "errors.h"

void exec_error(char *name, int status)
{
	write(2, name, strlen(name));
	write(2, ": ", 2);
	print_error(status);
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


}