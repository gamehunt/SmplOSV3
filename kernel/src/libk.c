#include <commons.h>
#include <shared.h>

#include <dev/serial.h>

#include <string.h>
#include <stdio.h>

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

    char buff[1024];
    memset(buff, 0, 1024);

    sprintf(&buff[0], "Assertion failed: %s in %s:%d", msg, file, line);

    k_serial_putstr(0x3F8, buff);
}

void __libk_putchar(char c){
    k_serial_putchar(0x3F8, c);
}