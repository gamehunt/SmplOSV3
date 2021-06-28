#include <string.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void * memcpy( void * destptr, const void * srcptr, size_t num ){
    uint8_t *buf = (uint8_t *)destptr;
    uint8_t *src = (uint8_t *)srcptr;
    while (num--)
    {
        *buf++ = *src++;
    }
    return destptr;
}

void * memset( void * memptr, uint8_t val, size_t num ){
    uint8_t *buf = (uint8_t *)memptr;
    while (num--)
    {
        *buf++ = (uint8_t)val;
    }
    return memptr;
}

char * strcpy(char *strDest, const char *strSrc)
{
    char *temp = strDest;
    while(*strDest++ = *strSrc++); 
    return temp;
}

int strlen(const char* str){
	int size = 0;
	while(str[size] != '\0'){
		size++;
	}
	return size;
}

int strncmp(const char *str1, const char *str2, size_t n)
{
	const char* end = str1+n;
    int result = 0;
    for(;result == 0 && str1 != end && (*str1 || *str2); result = *(str1++)-*(str2++));
    return result;
}