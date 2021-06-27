#ifndef __K_SMPLOS_MEMORY_H
#define __K_SMPLOS_MEMORY_H

#include <util.h>
#include <stdint.h>

#define PMM_STACK_ADDRESS                                  0x300000

#define PML4_MAPPING                                       0xFFFFFFFFFFFFF000
#define PDP_MAPPING(idx)                                 (0xFFFFFFFFFFE00000 + 0x1000*(idx))
#define PD_MAPPING(pdp_idx, pd_idx)                      (0xFFFFFFFFC0000000 + 0x200000*(pdp_idx)   + 0x1000*(pd_idx))
#define PT_MAPPING(pdp_idx, pd_idx, pt_idx)              (0xFFFFFF8000000000 + 0x40000000*(pdp_idx) + 0x200000*(pd_idx) + 0x1000*(pt_idx))

#define PAGE_FLAG_PRESENT                                  0x1
#define PAGE_FLAG_WRITABLE                                 0x2

CH_START

#pragma region pmm

uint64_t falloc();
void     pmm_mark_frame(uint64_t frame);
uint32_t pmm_get_stack_size();
void     pmm_init_metadata_bitmap();

#pragma endregion pmm

#pragma region paging

void     init_paging();
void     map(uint64_t physical, uint64_t virtual);

#pragma endregion

CH_END

#endif
