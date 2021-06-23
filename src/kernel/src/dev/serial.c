#include <dev/serial.h>
#include <io.h>

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}

void serial_putchar(char c){
   while (is_transmit_empty() == 0);
   outb(PORT, c);
}