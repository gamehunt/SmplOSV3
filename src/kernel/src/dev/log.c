#include <dev/serial.h>
#include <dev/log.h>
#include <stdarg.h>
#include <stdio.h>

void log(int level, const char *format, va_list args){
    switch(level){
        case K_KERNEL_LOGLEVEL_INFO:
            printf("[I] ");
            break; 
        case K_KERNEL_LOGLEVEL_DEBUG:
        #if defined(K_ENABLE_DEBUG)
            printf("[D] ");
        #else
            return;
        #endif
            break;
        case K_KERNEL_LOGLEVEL_ERROR:
            printf("[E] ");
            break;
        case K_KERNEL_LOGLEVEL_WARNING:
            printf("[W] ");
            break;
    }

    vprintf(format, args);
    printf("\r\n");
}

void info(const char *format, ...){
    va_list ap;
    va_start(ap, format);
    log(K_KERNEL_LOGLEVEL_INFO, format, ap);
    va_end(ap);
}
void debug(const char *format, ...){
    va_list ap;
    va_start(ap, format);
    log(K_KERNEL_LOGLEVEL_DEBUG, format, ap);
    va_end(ap);
}
void error(const char *format, ...){
    va_list ap;
    va_start(ap, format);
    log(K_KERNEL_LOGLEVEL_ERROR, format, ap);
    va_end(ap);
}

void warning(const char *format, ...){
    va_list ap;
    va_start(ap, format);
    log(K_KERNEL_LOGLEVEL_WARNING, format, ap);
    va_end(ap);
}