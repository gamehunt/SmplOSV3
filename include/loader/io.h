#pragma once

#if !defined(K_SMPLOS_LOADER)
#warning "These headers are for internal purposes and shouldnt be used outside of SmplOSV3 loader until you exactly know what are you doing!"
#endif

#include <stdint.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);