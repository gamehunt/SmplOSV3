#ifndef K_SMPLOS_LOG_H
#define K_SMPLOS_LOG_H

#include <stdarg.h>

#if defined(K_SMPLOS_KERNEL)
    #include <util.h>
#else
    #include <kernel/util.h>
#endif

#define K_KERNEL_LOGLEVEL_INFO  1
#define K_KERNEL_LOGLEVEL_DEBUG 2
#define K_KERNEL_LOGLEVEL_ERROR 3
#define K_KERNEL_LOGLEVEL_WARNING 4

CH_START

void info( const char *format, ...);
void debug(const char *format, ...);
void error(const char *format, ...);
void warning(const char *format, ...);
void log(int level, const char *format, va_list args);

CH_END

#endif