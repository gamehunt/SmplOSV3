#include <memory.h>

extern void gdt_load();

struct gdt_entry gdt[5];
struct gdt_ptr gp;

uint64_t   pml4[512]         __attribute__((aligned(0x1000)));
uint64_t   pd_ptr_table[512] __attribute__((aligned(0x1000)));
uint64_t   pd[512]           __attribute__((aligned(0x1000)));
uint64_t   pt[512]           __attribute__((aligned(0x1000)));
uint64_t   pt1[512]           __attribute__((aligned(0x1000)));

void add_gdt_entry(int num,uint32_t base,uint32_t limit,uint32_t access,uint32_t gran) {
  gdt[num].base_low = base;
  gdt[num].base_middle = (base >> 16);
  gdt[num].base_high = (base >> 24);
  gdt[num].limit = limit;
  gdt[num].granularity = gran;
  gdt[num].access = access;
}

void gdt_install() {
  info("Installing gdt");
  gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
  gp.base = (uint32_t)&gdt;
  add_gdt_entry(0, 0, 0, 0, 0);//0x0
  add_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);//0x8
  add_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);//0x10
  add_gdt_entry(3, 0, 0xFFFFFFFF, 0x92, 0xAF);//0x18
  add_gdt_entry(4, 0, 0xFFFFFFFF, 0x92, 0xCF);//0x20
  gdt_load();
}

void paging_setup(){
  info("Setting up 64bit paging");

  memset(pml4, 0, 4096);
  memset(pd_ptr_table, 0, 4096);
  memset(pd, 0, 4096);
  memset(pt, 0, 4096);
  memset(pt1, 0, 4096);

  pml4[0]         = (uint64_t)&pd_ptr_table  | 3;
  pd_ptr_table[0] = (uint64_t)&pd            | 3;
  pd[0]           = (uint64_t)&pt            | 3;
  pd[1]           = (uint64_t)&pt1           | 3;

  //ident map us and our kernel
  for(int i=0;i<512;i++){
    pt[i]           = 0x1000*i | 3;
  }

  for(int i=0;i<512;i++){
    pt1[i]           = (0x200000 + 0x1000*i) | 3;
  }
}