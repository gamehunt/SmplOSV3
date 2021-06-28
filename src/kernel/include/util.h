#ifndef __K_SMPLOS_UTIL_H
#define __K_SMPLOS_UTIL_H

#define SUPPRESS_UNUSED(x) (void)(x)

#define KB(x) (x*1024)
#define MB(x) (KB(x)*1024)

#ifdef __cplusplus
	#define CH_START extern "C" {
	#define CH_END } 
#else
	#define CH_START 
	#define CH_END 
#endif

#include <stdint.h>

void halt();
uint8_t endswith(const char *str, const char *suffix);

#endif