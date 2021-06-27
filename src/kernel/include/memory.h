#ifndef __K_SMPLOS_MEMORY_H
#define __K_SMPLOS_MEMORY_H

#include <util.h>
#include <stdint.h>

#define PMM_STACK_ADDRESS                                  0x300000

#define PML4_MAPPING                                       0xFFFFFFFFFFFFF000
#define PDP_MAPPING(idx)                                  (0xFFFFFFFFFFE00000 + 0x1000*(idx))
#define PD_MAPPING(pdp_idx, pd_idx)                       (0xFFFFFFFFC0000000 + 0x200000*(pdp_idx)   + 0x1000*(pd_idx))
#define PT_MAPPING(pdp_idx, pd_idx, pt_idx)               (0xFFFFFF8000000000 + 0x40000000*(pdp_idx) + 0x200000*(pd_idx) + 0x1000*(pt_idx))

#define PAGE_FLAG_PRESENT                                  0x1
#define PAGE_FLAG_WRITABLE                                 0x2
#define PAGE_FLAG_USER                                     0x4

#define CHECK_ADDR(x) (to_physical(x) != 0)

#define HEAP_START 0x10000000

#define PTR(addr) ((void*)((uint64_t)addr       + sizeof(memblock_t)))
#define HDR(addr) ((memblock_t*)((uint64_t)addr - sizeof(memblock_t)))

CH_START

struct memblock{
    char magic[3];
    struct memblock* next;
    struct memblock* prev;
    uint64_t size;
} __attribute__((packed));

typedef struct memblock memblock_t;

struct tss_entry_struct
{
    uint32_t reserved0;
    uint64_t rsp0;      // rsp when entering ring 0
    uint64_t rsp1;      // rsp when entering ring 1
    uint64_t rsp2;      // rsp when entering ring 2
    uint64_t reserved1;
    // The next 7 entries are the "Interrupt stack Table"
    // Here we can define stack pointers to use when handling interrupts.
    // Which one to use is defined in the Interrupt Descriptor Table.
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap;
} __attribute__((packed));
typedef struct tss_entry_struct tss_entry_t;

#pragma region pmm

uint64_t falloc();
void     ffree(uint64_t frame);
void     pmm_mark_frame(uint64_t frame);
uint32_t pmm_get_stack_size();

#pragma endregion pmm

#pragma region paging

void     init_paging();
void     map(uint64_t physical, uint64_t virtual);
void     mapf(uint64_t physical, uint64_t virtual, uint8_t flags);
void     unmap(uint64_t virtual);
uint64_t to_physical(uint64_t addr);

#pragma endregion

#pragma region heap

void    init_heap(uint64_t size);

void*   kalloc (uint64_t start, uint64_t size, uint8_t contigious);
void*   kmalloc(uint64_t size);
void    kfree  (void* mem);

void    print_free_list();

#pragma endregion

CH_END

#endif
