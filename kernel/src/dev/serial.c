#include <dev/serial.h>
#include <dev/io.h>

int k_serial_is_transmit_empty(int port) {
   return inb(port + 5) & 0x20;
}

void k_serial_putchar(int port, char c){
   while (k_serial_is_transmit_empty(port) == 0);
   outb(port, c);
}