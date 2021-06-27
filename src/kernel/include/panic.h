#include <util.h>

#ifndef __K_SMPLOS_PANIC_H
#define __K_SMPLOS_PANIC_H

#include <types/registers.h>

CH_START

void panic(regs_t dump, const char* message);

CH_END

#endif