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

#pragma region pmm

uint64_t falloc();
void     ffree(uint64_t frame);
void     pmm_mark_frame(uint64_t frame);
uint32_t pmm_get_stack_size();

#pragma endregion pmm

#pragma region paging

void     init_paging();
void     map(uint64_t physical, uint64_t virtual);
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
