//
// Created by Dominik Grybos on 12.11.2017.
//

#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "reader.h"

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