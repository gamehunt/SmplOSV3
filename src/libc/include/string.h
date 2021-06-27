#pragma once

#include <stddef.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2);

void * memcpy( void * destptr, const void * srcptr, size_t num );
void * memset( void * memptr, uint8_t val, size_t num );