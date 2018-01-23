#ifndef UTIL_H_
#define	UTIL_H_

void
fatal(char *message);

void *
ec_malloc(unsigned int size);

void
m_strcpy(char *dest, size_t destsz, char *src);

void
m_strncat(char *dest, size_t destsz, char *src, size_t srcsz);

void
m_sprintf(char *dest, size_t destsz, const char *format, ...);

#endif // UTIL_H_
