#pragma once

#define PORT 0x3f8 

int serial_init();
int is_transmit_empty();
void serial_putchar(char c);