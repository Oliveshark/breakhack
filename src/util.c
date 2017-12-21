#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MINGW32__
#include <string_s.h>
#endif // __MINGW32__
#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32

#include "util.h"

void
m_strcpy(char *dest, size_t destsz, char *src)
{
#ifndef _WIN32
	UNUSED(destsz);
	strcpy(dest, src);
#else
	strcpy_s(dest, destsz, src);
#endif // _WIN32
}

void
m_strncat(char *dest,
	 size_t destsz,
	 char *src,
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

	m_strcpy(error_message, 100, "[!!] Fatal Error ");
	m_strncat(error_message, 100, message, 83);
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
