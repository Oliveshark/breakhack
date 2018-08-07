/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>

#include "defines.h"

#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32

#include "util.h"

void
m_strcpy(char *dest, size_t destsz, const char *src)
{
#ifndef _MSC_VER
	UNUSED(destsz);
	strncpy(dest, src, destsz);
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
	vsnprintf(dest, destsz, format, args);
#else // _MSC_VER
	vsprintf_s(dest, destsz, format, args);
#endif // _MSC_VER
	va_end(args);
}

void
m_vsprintf(char *dest, size_t sz, const char *fmt, va_list args)
{
#ifndef _MSC_VER
	UNUSED (sz);
	vsnprintf(dest, sz, fmt, args);
#else // _MSC_VER
	vsprintf_s(dest, sz, fmt, args);
#endif // _MSC_VER
}

void
log_print(FILE *out,
	  const char *prefix,
	  const char *file,
	  int line,
	  const char *function,
	  const char * fmt,
	  ...)
{
	va_list args;
	char tstamp[10];

	timestamp(tstamp, 10);
#ifdef DEBUG
#ifndef _WIN32
	if (out == stdout || out == stderr) {
		fprintf(out, "\033[34m[%s]", tstamp);
		if (strcmp(prefix, "DEBUG") == 0)
			fprintf(out, "\033[33m");
		else if (strcmp(prefix, "FATAL") == 0 || strcmp(prefix, "ERROR") == 0)
			fprintf(out, "\033[31m");
		else
			fprintf(out, "\033[32m");
		fprintf(out, "[%5s]", prefix);
		fprintf(out, "\033[36m[%20s:%-3d]\033[37m[%20s()]\033[0m ",
			file, line, function);
	} else {
		fprintf(out, "[%s][%5s][%20s:%-3d][%20s()] ", tstamp, prefix, file, line, function);
	}
#else // _WIN32
	fprintf(out, "[%s][%5s][%20s:%-3d][%20s()] ", tstamp, prefix, file, line, function);
#endif // _WIN32
#else // DEBUG
	UNUSED(prefix);
	UNUSED(file);
	UNUSED(line);
	UNUSED(function);
#endif // DEBUG
	va_start(args, fmt);
	vfprintf(out, fmt, args);
	va_end(args);
	printf("\n");
}

void *
ec_malloc(unsigned long size)
{
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL)
		fatal("in ec_malloc() on memory allocation");
	return ptr;
}

void *
ec_calloc(size_t count, unsigned long size)
{
	void *ptr;
	ptr = calloc(count, size);
	if (ptr == NULL)
		fatal("in ec_calloc() on memory allocation");
	return ptr;
}

void
timestamp(char *tstamp, size_t sz)
{
	time_t cTime;
	struct tm *tm_info;

	time(&cTime);
#ifdef _MSC_VER
	tm_info = ec_malloc(sizeof(struct tm));
	localtime_s(tm_info, &cTime);
#else // _MSC_VER
	tm_info = localtime(&cTime);
#endif // _MSC_VER

	strftime(tstamp, sz, "%H:%M:%S", tm_info);

#ifdef _MSC_VER
	free(tm_info);
#endif // _MSC_VER
}

char *
to_lower(const char *str)
{
	char *lcstr;
	unsigned int i;

	lcstr = ec_malloc(((unsigned int) strlen(str) + 1) * sizeof(char));

	for (i = 0; i < strlen(str); ++i) {
		lcstr[i] = (char) tolower(str[i]);
	}
	lcstr[i] = '\0';

	return lcstr;
}
