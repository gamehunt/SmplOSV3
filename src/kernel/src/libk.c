#include <stdio.h>
#include <dev/serial.h>
#include <panic.h>

void putchar(char c){ //TODO __libk_putchar()
    serial_putchar(c);
}

void * __libk_malloc( size_t sizemem ){
    return kmalloc(sizemem);
}

void __libk_free( void* sizemem ){
    kfree(sizemem);
}

void __libk_assertion_fail(const char *msg, const char *file, int line){
    char message[1024];
    sprintf(message, "Assertion failed: %s in %s at line %d", msg, file, line);
    panic(NULL, message);
}