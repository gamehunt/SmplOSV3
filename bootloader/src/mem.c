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

BSTATUS b_map_range(uint64_t start, uint64_t size_in_pages, uint8_t ident){ //TODO fix

    b_info("Allocating: 0x%x - 0x%x", start, start + size_in_pages * 0x1000);

    uint32_t v_hi = start >> 32;
    uint32_t v_lo = (uint32_t) start;

    uint16_t pml4_idx = (v_hi >> 7) & 0x1FF;
    uint16_t pdp_idx = ((v_hi << 2) & 0x1FC) | ((v_lo >> 30) & 0x3);
    uint16_t pd_idx = (v_lo >> 21) & 0x1FF;
    uint16_t pt_idx = (v_lo >> 12) & 0x1FF;

    uint64_t phys;
    if(ident){
        phys = start;
    }else{
        BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&phys));
    }

    
    if (!(pml[pml4_idx] & 1))
    {
        uint64_t addr;
        BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&addr));
        pml[pml4_idx] = addr | 3;
        memset((uint8_t*)PDP_MAPPING(pml4_idx), 0, 4096);
    }
    else
    {
        pml[pml4_idx] |= 3;
    }

    uint64_t *strct = (uint64_t *)PDP_MAPPING(pml4_idx);

    if (!(strct[pdp_idx] & 1))
    {
        uint64_t addr;
        BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&addr));
        strct[pdp_idx] = addr | 3;
        memset((uint8_t*)PD_MAPPING(pml4_idx, pdp_idx), 0, 4096);
    }
    else
    {
        strct[pdp_idx] |= 3;
    }

    strct = (uint64_t *)PD_MAPPING(pml4_idx, pdp_idx);

    if (!(strct[pd_idx] & 1))
    {
        uint64_t addr;
        BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&addr));
        strct[pd_idx] = addr | 3;
        memset((uint8_t*)PT_MAPPING(pml4_idx, pdp_idx, pd_idx), 0, 4096);
    }
    else
    {
        strct[pd_idx] |= 3;
    }

    strct = (uint64_t *)PT_MAPPING(pml4_idx, pdp_idx, pd_idx);
    strct[pt_idx] = phys | 3;

    return 0;
}

void b_setup_paging(){

    uint64_t* l_pml;
    BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pml));
    memset(l_pml, 0, 0x1000);
    l_pml[511] = (uint64_t)l_pml | 3;

    uint64_t* l_pdp;
    BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pdp));
    memset(l_pdp, 0, 0x1000);
    l_pml[0] = (uint64_t)l_pdp | 3;

    for(int p=0;p<4;p++){
        uint64_t* l_pd;
        BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pd));
        memset(l_pd, 0, 0x1000);
        l_pdp[p] = (uint64_t)l_pd | 3;

        for(uint32_t i=0;i<512;i++){
            uint64_t* l_pt;
            BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, 1, (uint64_t*)(&l_pt));
            l_pd[i] = (uint64_t)l_pt | 3;
            for(uint32_t j=0;j<512;j++){
                l_pt[j] = (i * 0x200000 + j * 0x1000) | 3;
            }
        }
    }

    b_set_pml((uint64_t)l_pml); 
}
