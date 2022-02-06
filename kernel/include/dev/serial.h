#ifndef _K_SERIAL_H
#define _K_SERIAL_H

int  k_serial_is_transmit_empty(int port);
void k_serial_putchar(int port, char c);

#endif