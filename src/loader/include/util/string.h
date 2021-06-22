# pragma once

#include <stddef.h>

int strcmp(const char* s1, const char* s2);

void * memcpy( void * destptr, const void * srcptr, size_t num );
void * memset( void * memptr, int val, size_t num );