#pragma once

#if !defined(K_SMPLOS_LOADER)
#warning "These headers are for internal purposes and shouldnt be used outside of SmplOSV3 loader until you exactly know what are you doing!"
#endif

#include <stdint.h>

struct gdt_entry {
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
}__attribute__((packed));

struct gdt_ptr {
  uint16_t pad;
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

void add_gdt_entry(int num,uint32_t base,uint32_t limit,uint32_t access,uint32_t gran);
void gdt_install();