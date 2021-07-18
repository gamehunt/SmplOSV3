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

char * strcat(char *dest, const char *src){
    char *rdest = dest;
    while (*dest)
      dest++;
    while (*dest++ = *src++)
        ;
    return rdest;
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

size_t strspn(const char* cs, const char* ct) {
  size_t n;
  const char* p;
  for(n=0; *cs; cs++, n++) {
    for(p=ct; *p && *p != *cs; p++)
      ;
    if (!*p)
      break;
  }
  return n;
}


char *strchr (const char *s, int c)
{
  do {
    if (*s == c)
      {
        return (char*)s;
      }
  } while (*s++);
  return (0);
}

size_t strcspn(const char *s1, const char *s2)
{
    unsigned int len =0;
    //return 0 if any one is NULL
    if((s1 == NULL) || (s2 == NULL))
        return len;
    //till not get null character
    while(*s1)
    {
        //return s1 char position if found in s2
        if(strchr(s2,*s1))
        {
            return len;
        }
        else
        {
            //increment s1
            s1++;
            //increment len variable
            len++;
        }
    }
    return len;
}

char* strtok(char *str, const char* delim) {
    static char* p=0;
    if(str){
        p = str;
    }else if(!p){
        return 0;
    }
    str = p   + strspn(p,delim);
    p   = str + strcspn(str,delim);
    if(p==str)
        return p=0;
    p = *p ? *p=0,p+1 : 0;
    return str;
}

char *strdup(char *src)
{
    int len = strlen(src);
    char * str = (char*)malloc(sizeof(char) * (len+1));
    strcpy(str, src);
    return (str);
}