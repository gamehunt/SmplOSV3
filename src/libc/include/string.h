#pragma once

#include <stddef.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2);
char * strcpy(char *strDest, const char *strSrc);
int strlen(const char* str);
int strncmp(const char *str1, const char *str2, size_t n);

void * memcpy( void * destptr, const void * srcptr, size_t num );
void * memset( void * memptr, uint8_t val, size_t num );