#include <stdint.h>

void _start(){
    kernel_main();
}

void outb(uint16_t port, uint8_t value) {  // 8bit port output
  asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void kernel_main(){
    outb(0x3F8, 'E');
    asm("hlt");
}