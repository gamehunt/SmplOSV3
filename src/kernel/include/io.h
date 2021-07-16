#ifndef __K_SMPLOS_IO_H
#define __K_SMPLOS_IO_H

#if defined(K_SMPLOS_KERNEL)
    #include <util.h>
#else
    #include <kernel/util.h>
#endif

#include <stdint.h>

CH_START

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

CH_END

#endif