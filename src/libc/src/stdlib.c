#include <stdlib.h>

extern void* __libk_malloc(size_t);
extern void  __libk_free(void*);

void * malloc( size_t sizemem ){
    #ifdef K_SMPLOS_LIBK
        return __libk_malloc(sizemem);
    #else
        return 0;
    #endif
}
void free(void* ptr){
    #ifdef K_SMPLOS_LIBK
        __libk_free(ptr);
    #else
        
    #endif
}
