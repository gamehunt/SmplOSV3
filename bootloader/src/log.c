#include <log.h>
#include <stdarg.h>

void b_vlog(BLOGLEVEL level, const char* format, va_list ptr){
    switch(level){
        case B_LOG_ERR:
            printf("[E]: ");
            break;
        case B_LOG_WARN:
            printf("[W]: ");
            break;
        case B_LOG_INFO:
        default:
            printf("[I]: ");
            break;
    }
    vprintf(format, ptr);
    printf("\n");
}

void b_log(BLOGLEVEL level, const char* format, ...){
    va_list ptr;
    va_start(ptr, format);
    b_vlog(level, format, ptr);
    va_end(ptr);
}

void b_info(const char* format, ...){
    va_list ptr;
    va_start(ptr, format);
    b_vlog(B_LOG_INFO, format, ptr);
    va_end(ptr);
}

void b_warn(const char* format, ...){
    va_list ptr;
    va_start(ptr, format);
    b_vlog(B_LOG_WARN, format, ptr);
    va_end(ptr);   
}

void b_error(const char* format, ...){
    va_list ptr;
    va_start(ptr, format);
    b_vlog(B_LOG_ERR, format, ptr);
    va_end(ptr);
}