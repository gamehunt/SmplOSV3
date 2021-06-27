#include <memory.h>
#include <log.h>
#include <util/string.h>

extern void gdt_load();

struct gdt_entry gdt[5];
struct gdt_entry gdt64[7];
struct gdt_ptr gp;
struct gdt_ptr gp64;
tss_entry_t    tss_entry;

uint64_t   pml4[512]           __attribute__((aligned(0x1000)));
uint64_t   pd_ptr_table[512]   __attribute__((aligned(0x1000)));
uint64_t   pd[512]             __attribute__((aligned(0x1000)));
uint64_t   pt[512]             __attribute__((aligned(0x1000)));
uint64_t   pt1[512]            __attribute__((aligned(0x1000)));
uint64_t   pt2[512]            __attribute__((aligned(0x1000)));
uint64_t   pt3[512]            __attribute__((aligned(0x1000)));
uint64_t   pt4[512]            __attribute__((aligned(0x1000)));
uint64_t   pt5[512]            __attribute__((aligned(0x1000)));

uint64_t    usr_stack[1024]     __attribute__((aligned(0x1000)));;;

void add_gdt_entry(int num,uint32_t base,uint32_t limit,uint32_t access,uint32_t gran) {
  gdt[num].base_low = base;
  gdt[num].base_middle = (base >> 16);
  gdt[num].base_high = (base >> 24);
  gdt[num].limit = limit;
  gdt[num].granularity = gran;
  gdt[num].access = access;
}

void add_gdt64_entry(int num,uint32_t base,uint32_t limit,uint32_t access,uint32_t gran) {
  gdt64[num].base_low = base;
  gdt64[num].base_middle = (base >> 16);
  gdt64[num].base_high = (base >> 24);
  gdt64[num].limit = limit;
  gdt64[num].granularity = gran;
  gdt64[num].access = access;
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
  gdt64_install();
}

static void write_tss(int32_t num, uint64_t rsp0) {
	tss_entry_t * tss = &tss_entry;
	uint32_t base = (uint32_t)tss;
	uint32_t limit = sizeof(*tss);
	add_gdt64_entry(num  , base, limit, 0xE9, 0x40);
  add_gdt64_entry(num+1,    0,     0,    0,    0);
	memset(tss, 0x0, sizeof(*tss));
	tss->rsp0 = rsp0;
	tss->iomap = 0xdfff;
}

void gdt64_install(){
  gp64.limit = (sizeof(struct gdt_entry) * 7) - 1;
  gp64.base = (uint32_t)&gdt64;
  add_gdt64_entry(0, 0, 0, 0, 0);               //0x0
  add_gdt64_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xAF);//0x8
  add_gdt64_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);//0x10
  add_gdt64_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xAF);//0x18
  add_gdt64_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);//0x20
  write_tss      (5, &usr_stack);
}

void paging_setup(){
  info("Setting up 64bit paging");

  memset(pml4, 0, 4096);
  memset(pd_ptr_table, 0, 4096);
  memset(pd, 0, 4096);

  //info("%x %x", &tss, sizeof(tss));
  //asm("hlt");

  pml4[0]         = (uint64_t)&pd_ptr_table  | 3;
  pd_ptr_table[0] = (uint64_t)&pd            | 3;
  pd[0]           = (uint64_t)&pt            | 3;
  pd[1]           = (uint64_t)&pt1           | 3;
  pd[2]           = (uint64_t)&pt2           | 3;
  pd[3]           = (uint64_t)&pt3           | 3;
  pd[4]           = (uint64_t)&pt4           | 3;
  pd[5]           = (uint64_t)&pt5           | 3;

  for(int i=0;i<512;i++){
    ((uint64_t*)pd[511])[i]            = 0x1000*i | 3;
  }

  //ident map us and our kernel + space for MMU
  for(int i=0;i<512;i++){
    pt[i]            = 0x1000*i | 3;
    pt1[i]           = (0x200000 +  0x1000*i) | 3;
    pt2[i]           = (0x400000 +  0x1000*i) | 3;
    pt3[i]           = (0x600000 +  0x1000*i) | 3;
    pt4[i]           = (0x800000 +  0x1000*i) | 3;
    pt5[i]           = (0xa00000 +  0x1000*i) | 3;
  }
}