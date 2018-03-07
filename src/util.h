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

#ifndef UTIL_H_
#define	UTIL_H_

#include <stdarg.h>

void
fatal(const char *fmt, ...);

void
error(const char *fmt, ...);

void
debug(const char *fmt, ...);

void
info(const char *fmt, ...);

void *
ec_malloc(unsigned long size);

void
m_strcpy(char *dest, size_t destsz, const char *src);

void
m_strncat(char *dest, size_t destsz, char *src, size_t srcsz);

void
m_sprintf(char *dest, size_t destsz, const char *format, ...);

void
m_vsprintf(char *dest, size_t sz, const char *fmt, va_list args);

void
timestamp(char *tstamp, size_t sz);

char *
to_lower(const char *str);

#endif // UTIL_H_
