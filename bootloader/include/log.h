#ifndef _BOOTLOADER_LOG_H
#define _BOOTLOADER_LOG_H

#define B_LOG_INFO  0
#define B_LOG_WARN  1
#define B_LOG_ERR   2

#include <commons.h>

void b_vlog(BLOGLEVEL level, const char* format, va_list ptr);
void b_log(BLOGLEVEL level, const char* format, ...);

void b_info(const char* format, ...);
void b_warn(const char* format, ...);
void b_error(const char* format, ...);

#endif