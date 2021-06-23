#include <stdio.h>
#include <dev/serial.h>

void putchar(char c){
    serial_putchar(c);
}