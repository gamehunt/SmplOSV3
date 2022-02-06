#ifndef __LIBC_STDLIB
#define __LIBC_STDLIB

#include <stdint.h>
#include <stddef.h>

void * malloc( size_t sizemem );
void     free( void* ptr      );

#endif