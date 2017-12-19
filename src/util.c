#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32
#include <string.h>

#include "util.h"

void
_strcpy(char *restrict dest, size_t destsz, char *restrict src)
{
#ifndef _WIN32
	UNUSED(destsz);
	strcpy(dest, src);
#else
	strcpy_s(dest, destsz, src);
#endif // _WIN32
}

void
_strncat(char *restrict dest,
	 size_t destsz,
	 char *restrict src,
	 size_t srcsz)
{
#ifndef _WIN32
	UNUSED(destsz);
	UNUSED(srcsz);
	strncat(dest, src, srcsz);
#else
	strncat_s(dest, destsz, src, srcsz);
#endif // _WIN32
}

void fatal(char *message)
{
	char error_message[100];

	_strcpy(error_message, 100, "[!!] Fatal Error ");
	_strncat(error_message, 100, message, 83);
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
