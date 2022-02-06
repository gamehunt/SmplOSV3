#include <commons.h>
#include <shared.h>

#include <dev/serial.h>

#include <string.h>

void* __libk_malloc(size_t s){
    UNUSED(s);
    return NULL;
}

void  __libk_free(void* p){
    UNUSED(p);
}

void  __libk_assertion_fail(const char *msg, const char *file, int line){
    UNUSED(file);
    UNUSED(line);


    k_serial_putstr(0x3F8, msg);
}

void __libk_putchar(char c){
    k_serial_putchar(0x3F8, c);
}