#ifndef UTIL_H_
#define	UTIL_H_

void fatal(char *message);

void *ec_malloc(unsigned int size);

void _strcpy(char *restrict dest, size_t destsz, char *restrict src);

void
_strncat(char *restrict dest,
		size_t destsz,
		char *restrict src,
		size_t srcsz);

#endif // UTIL_H_
