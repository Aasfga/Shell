//
// Created by Dominik Grybos on 12.11.2017.
//

#include <unistd.h>
#include <fcntl.h>
#include "debug.h"

void swap_stdin()
{
	close(0);
	int i = open(FILE, O_RDONLY, 0);
	i += 0;
}
