#pragma once

#include <stdint.h>

#define PORT 0x3f8 

int is_transmit_empty();
void serial_putchar(char c);