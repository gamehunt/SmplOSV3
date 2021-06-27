#include <memory.h>

uint64_t   pml4[512]               __attribute__((aligned(0x1000)));
uint64_t   pd_ptr_table[512]       __attribute__((aligned(0x1000)));
uint64_t   pd[512]                 __attribute__((aligned(0x1000)));
uint64_t   pt[10][512]             __attribute__((aligned(0x1000)));

extern void set_pml(uint64_t* pml);
extern void invl_page(uint64_t* pg);
extern void flush_tlb();

void     init_paging(){

    memset(pml4, 0, 4096);
    memset(pd_ptr_table, 0, 4096);
    memset(pd, 0, 4096);

    pml4[511]       = (uint64_t)&pml4            | 3;
    pml4[0]         = (uint64_t)&pd_ptr_table    | 3;
    pd_ptr_table[0] = (uint64_t)&pd              | 3;

    for(int i=0;i<10;i++){
        pd[i]  = (uint64_t)&pt[i] | 3;
    }

    for(int j=0;j<10;j++){
        for(int i=0;i<512;i++){
            pt[j][i]            = 0x200000*j + 0x1000*i | 3;
        }
    }

    set_pml(pml4);
}

void map(uint64_t physical, uint64_t virtual){

    uint32_t v_hi     = virtual  >> 32;
    uint32_t v_lo     = (uint32_t)virtual;

    uint16_t pml4_idx =  (v_hi >> 7) & 0x1FF;
    uint16_t pdp_idx  =  ((v_hi<< 2) & 0x1FC) | ((v_lo >> 30) & 0x3);
    uint16_t pd_idx   =  (v_lo >> 21) & 0x1FF;
    uint16_t pt_idx   =  (v_lo >> 12) & 0x1FF;

    if(!(pml4[pml4_idx] & PAGE_FLAG_PRESENT)){
        pml4[pml4_idx] = falloc() | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE;
        memset(PDP_MAPPING(pml4_idx), 0, 4096);
    }

    uint64_t* strct = (uint64_t*)PDP_MAPPING(pml4_idx);

    if(!(strct[pdp_idx] & PAGE_FLAG_PRESENT)){
        strct[pdp_idx] = falloc() | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE;
        memset(PD_MAPPING(pml4_idx, pdp_idx), 0, 4096);
    }

    strct = (uint64_t*)PD_MAPPING(pml4_idx, pdp_idx);

    if(!(strct[pd_idx] & PAGE_FLAG_PRESENT)){
        strct[pd_idx] = falloc() | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE;
        flush_tlb();
        memset(PT_MAPPING(pml4_idx, pdp_idx, pd_idx), 0, 4096);
    }

    strct = (uint64_t*)PT_MAPPING(pml4_idx, pdp_idx, pd_idx);
    strct[pt_idx] = physical | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE;
}
