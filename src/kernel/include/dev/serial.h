#ifndef K_SMPLOS_SERIAL_H
#define K_SMPLOS_SERIAL_H

#include <stdint.h>
#include <util.h>

#define PORT 0x3f8 

CH_START

int is_transmit_empty();
void serial_putchar(char c);

CH_END

#endif