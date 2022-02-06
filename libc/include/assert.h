#ifndef __LIBC_ASSERT
#define __LIBC_ASSERT

#define STRIPPATH(s)\
    (sizeof(s) > 2 && (s)[sizeof(s)-2] == '/' ? (s) + sizeof(s) - 1 : \
    sizeof(s) > 3 && (s)[sizeof(s)-3] == '/' ? (s) + sizeof(s) - 2 : \
    sizeof(s) > 4 && (s)[sizeof(s)-4] == '/' ? (s) + sizeof(s) - 3 : \
    sizeof(s) > 5 && (s)[sizeof(s)-5] == '/' ? (s) + sizeof(s) - 4 : \
    sizeof(s) > 6 && (s)[sizeof(s)-6] == '/' ? (s) + sizeof(s) - 5 : \
    sizeof(s) > 7 && (s)[sizeof(s)-7] == '/' ? (s) + sizeof(s) - 6 : \
    sizeof(s) > 8 && (s)[sizeof(s)-8] == '/' ? (s) + sizeof(s) - 7 : \
    sizeof(s) > 9 && (s)[sizeof(s)-9] == '/' ? (s) + sizeof(s) - 8 : \
    sizeof(s) > 10 && (s)[sizeof(s)-10] == '/' ? (s) + sizeof(s) - 9 : \
    sizeof(s) > 11 && (s)[sizeof(s)-11] == '/' ? (s) + sizeof(s) - 10 : (s))

#ifdef SHORT_ASSERT
#define __FILENAME__ STRIPPATH(__FILE__)
#else
#define __FILENAME__ __FILE__
#endif

#ifndef NDEBUG
    #define assert(expr) (void)((expr) || (__assert (#expr, __FILENAME__, __LINE__),0))
#else
    #define assert(ignore)((void) 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void __assert (const char *msg, const char *file, int line);

#ifdef __cplusplus
};
#endif

#endif