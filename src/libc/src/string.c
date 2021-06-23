#include <string.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void * memcpy( void * destptr, const void * srcptr, size_t num ){
    uint32_t *buf = (uint32_t *)destptr;
    uint32_t *src = (uint32_t *)srcptr;
    while (num--)
    {
        *buf++ = *src++;
    }
    return destptr;
}

void * memset( void * memptr, int val, size_t num ){
    uint32_t *buf = (uint32_t *)memptr;
    while (num--)
    {
        *buf++ = (uint32_t)val;
    }
    return memptr;
}