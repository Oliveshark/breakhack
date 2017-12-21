#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32

#include "util.h"

void
m_strcpy(char *dest, size_t destsz, char *src)
{
#ifndef _MSC_VER
	UNUSED(destsz);
	strcpy(dest, src);
#else // _MSC_VER
	strcpy_s(dest, destsz, src);
#endif // _MSC_VER
}

void
m_strncat(char *dest,
	 size_t destsz,
	 char *src,
	 size_t srcsz)
{
#ifndef _MSC_VER
	UNUSED(destsz);
	UNUSED(srcsz);
	strncat(dest, src, srcsz);
#else // _MSC_VER
	strncat_s(dest, destsz, src, srcsz);
#endif // _MSC_VER
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
