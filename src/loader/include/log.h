#pragma once

#if !defined(K_SMPLOS_LOADER)
#warning "These headers are for internal purposes and shouldnt be used outside of SmplOSV3 loader until you exactly know what are you doing!"
#endif

#include <stdarg.h>

#define K_LOADER_LOGLEVEL_INFO  1
#define K_LOADER_LOGLEVEL_DEBUG 2
#define K_LOADER_LOGLEVEL_ERROR 3
#define K_LOADER_LOGLEVEL_WARNING 4

void info( const char *format, ...);
void debug(const char *format, ...);
void error(const char *format, ...);
void log(int level, const char *format, va_list args);