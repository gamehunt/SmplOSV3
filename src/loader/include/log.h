#pragma once

#include <stdarg.h>

#define K_LOADER_LOGLEVEL_INFO  1
#define K_LOADER_LOGLEVEL_DEBUG 2
#define K_LOADER_LOGLEVEL_ERROR 3

void info( const char *format, ...);
void debug(const char *format, ...);
void error(const char *format, ...);
void log(int level, const char *format, va_list args);