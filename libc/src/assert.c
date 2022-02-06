#include <assert.h> 

extern void  __libk_assertion_fail(const char *msg, const char *file, int line);

void __assert (const char *msg, const char *file, int line){
    #ifdef _SMPLOS_LIBK
        __libk_assertion_fail(msg, file, line);
    #else

    #endif
}