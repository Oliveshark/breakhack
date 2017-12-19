#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WINDOWS
#include <unistd.h>
#endif
#include <string.h>

#include "util.h"

void fatal(char *message)
{
	char error_message[100];

	strcpy_s(error_message, 100, "[!!] Fatal Error ");
	strncat_s(error_message, 100, message, 83);
	perror(error_message);
	exit(-1);
}

void *ec_malloc(unsigned int size)
{
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL)
		fatal("in ec_malloc() on memory allocation");
	return ptr;
}
