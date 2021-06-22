# pragma once

#if !defined(K_SMPLOS_LOADER)
#warning "These headers are for internal purposes and shouldnt be used outside of SmplOSV3 loader until you exactly know what are you doing!"
#endif

#include <stddef.h>

int strcmp(const char* s1, const char* s2);

void * memcpy( void * destptr, const void * srcptr, size_t num );
void * memset( void * memptr, int val, size_t num );