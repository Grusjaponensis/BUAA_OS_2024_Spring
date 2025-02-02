#ifndef _BLIB_H
#define _BLIB_H

#include <blib_macros.h>
#include <machine.h>

typedef unsigned int size_t;
#define NULL 0


size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
char *strcat(char *dst, const char *src);
char *strncat(char *dest, const char *src, size_t n); 
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *str, int character);
char* strsep(char** stringp, const char* delim);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif
