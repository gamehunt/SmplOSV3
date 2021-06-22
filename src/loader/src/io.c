#include <io.h>

void outb(uint16_t port, uint8_t value) {  // 8bit port output
  asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}
 
uint8_t inb(uint16_t port) {  // 8bit port input
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}