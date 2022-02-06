#include <dev/serial.h>
#include <dev/io.h>

#include <string.h>

int k_serial_is_transmit_empty(int port) {
   return inb(port + 5) & 0x20;
}

void k_serial_putchar(int port, char c){
   while (k_serial_is_transmit_empty(port) == 0);
   outb(port, c);
}

void k_serial_putstr(int port, const char* s){
    for(int i=0; i<strlen(s);i++){
        k_serial_putchar(port, s[i]);
    }
}