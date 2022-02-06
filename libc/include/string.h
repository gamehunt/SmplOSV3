#ifndef __LIBC_STRING
#define __LIBC_STRING

#include <stddef.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2);
char * strcpy(char *strDest, const char *strSrc);
char * strcat(char *dest, const char *src);
int strlen(const char* str);
int strncmp(const char *str1, const char *str2, size_t n);
char* strtok(char *str, const char* delim);
char *strdup(char *src);
size_t strspn(const char* cs, const char* ct);
size_t strcspn(const char *s1, const char *s2);
char *strchr(const char *s, int c);

void * memcpy( void * destptr, const void * srcptr, size_t num );
void * memset( void * memptr, uint8_t val, size_t num );

#endif