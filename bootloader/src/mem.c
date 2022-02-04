#include <mem.h>
#include <shared.h>
#include <log.h>

#define PML4_MAPPING                                       0xFFFFFFFFFFFFF000
#define PDP_MAPPING(idx)                                  (0xFFFFFFFFFFE00000 + 0x1000*(idx))
#define PD_MAPPING(pdp_idx, pd_idx)                       (0xFFFFFFFFC0000000 + 0x200000*(pdp_idx)   + 0x1000*(pd_idx))
#define PT_MAPPING(pdp_idx, pd_idx, pt_idx)               (0xFFFFFF8000000000 + 0x40000000*(pdp_idx) + 0x200000*(pd_idx) + 0x1000*(pt_idx))

static uint64_t* pml = NULL;

extern void __b_asm_set_pml(uint64_t);

void b_set_pml(uint64_t addr){
    __b_asm_set_pml(addr);
    pml = (uint64_t*)addr;
}

uint64_t* b_get_pml(){
    return pml;
}

//TODO fix
void b_setup_paging(){

    uint64_t* l_pml;
    BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pml));
    memset(l_pml, 0, 0x1000);
    l_pml[511] = (uint64_t)l_pml | 3;

    uint64_t* l_pdp;
    BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pdp));
    memset(l_pdp, 0, 0x1000);
    l_pml[0] = (uint64_t)l_pdp | 3;

    for(int p=0;p<128;p++){
        uint64_t* l_pd;
        BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pd));
        memset(l_pd, 0, 0x1000);
        l_pdp[p] = (uint64_t)l_pd | 3;

        for(uint32_t i=0;i<512;i++){
            uint64_t* l_pt;
            BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pt));
            l_pd[i] = (uint64_t)l_pt | 3;
            for(uint32_t j=0;j<512;j++){
                l_pt[j] = (p * 0x40000000 + i * 0x200000 + j * 0x1000) | 3;
            }
        }
    }

    b_set_pml((uint64_t)l_pml);    
}
