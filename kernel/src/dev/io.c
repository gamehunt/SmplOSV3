#include <dev/io.h>

void outb(uint16_t port, uint8_t value) {  // 8bit port output
  __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void outw(uint16_t port, uint16_t value) {  // 16bit port output
  __asm__ volatile("outw %1, %0" : : "dN"(port), "a"(value));
}

void outl(uint16_t port, uint32_t value) {  // 32bit port output
  __asm__ volatile("outl %1, %0" : : "dN"(port), "a"(value));
}

uint8_t inb(uint16_t port) {  // 8bit port input
  uint8_t ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

uint16_t inw(uint16_t port) {  // 16bit port input
  uint16_t ret;
  __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

uint32_t inl(uint16_t port) {  // 32bit port input
  uint32_t ret;
  __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}