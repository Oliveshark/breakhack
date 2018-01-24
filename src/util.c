#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "defines.h"

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

void
m_sprintf(char * dest, size_t destsz, const char * format, ...)
{
	va_list args;

	va_start(args, format);
#ifndef _MSC_VER
	UNUSED(destsz);
	vsprintf(dest, format, args);
#else // _MSC_VER
	vsprintf_s(dest, destsz, format, args);
#endif // _MSC_VER
	va_end(args);
}

void
debug(const char *fmt, ...)
{
	va_list args;
	char tstamp[10];

	timestamp(tstamp, 10);
	printf("[%s][--] ", tstamp);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}

void
info(const char * fmt, ...)
{
	va_list args;
	char tstamp[10];

	timestamp(tstamp, 10);
	printf("[%s][**] ", tstamp);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}

void
error(const char *fmt, ...)
{
	va_list args;
	char tstamp[10];

	timestamp(tstamp, 10);
	fprintf(stderr, "[%s][!*] ", tstamp);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
}

void
fatal(const char *fmt, ...)
{
	va_list args;
	char tstamp[10];

	timestamp(tstamp, 10);
	fprintf(stderr, "[%s][!!] ", tstamp);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(-1);
}

void
*ec_malloc(unsigned int size)
{
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL)
		fatal("in ec_malloc() on memory allocation");
	return ptr;
}

void
timestamp(char *tstamp, size_t sz)
{
	time_t cTime;
	struct tm *tm_info;

	time(&cTime);
	tm_info = localtime(&cTime);

	strftime(tstamp, sz, "%H:%M:%S", tm_info);
}
