#include <memory.h>
#include <string.h>

static uint64_t   pml4[512]               __attribute__((aligned(0x1000)));
static uint64_t   pd_ptr_table[512]       __attribute__((aligned(0x1000)));
static uint64_t   pd[512]                 __attribute__((aligned(0x1000)));
static uint64_t   pt[5][512]              __attribute__((aligned(0x1000)));

extern void set_pml(uint64_t* pml);
extern void invl_page(uint64_t* pg);
extern void flush_tlb();

static uint64_t* current_pml4 = pml4;

void     init_paging(){

    memset(pml4, 0, 4096);
    memset(pd_ptr_table, 0, 4096);
    memset(pd, 0, 4096);

    pml4[511]       = (uint64_t)&pml4            | 3;
    pml4[0]         = (uint64_t)&pd_ptr_table    | 3;
    pd_ptr_table[0] = (uint64_t)&pd              | 3;

    for(int i=0;i<5;i++){
        pd[i]  = (uint64_t)&pt[i] | 3;
    }

    for(int j=0;j<5;j++){
        for(int i=0;i<512;i++){
            pt[j][i]            = (0x200000*j + 0x1000*i) | 3;
        }
    }

    set_pml(pml4);
}

void map(uint64_t physical, uint64_t virtual){
    mapf(physical, virtual, PAGE_FLAG_WRITABLE);
}

void mapf(uint64_t physical, uint64_t virtual, uint16_t flags){

    uint32_t v_hi     = virtual  >> 32;
    uint32_t v_lo     = (uint32_t)virtual;

    uint16_t pml4_idx =  (v_hi >> 7) & 0x1FF;
    uint16_t pdp_idx  =  ((v_hi<< 2) & 0x1FC) | ((v_lo >> 30) & 0x3);
    uint16_t pd_idx   =  (v_lo >> 21) & 0x1FF;
    uint16_t pt_idx   =  (v_lo >> 12) & 0x1FF;

    if(!(current_pml4[pml4_idx] & PAGE_FLAG_PRESENT)){
        current_pml4[pml4_idx] = falloc() | PAGE_FLAG_PRESENT | flags;
        memset(PDP_MAPPING(pml4_idx), 0, 4096);
    }else{
        current_pml4[pml4_idx] &= 0xFFFFFFFFFFFFFF001; 
        current_pml4[pml4_idx] |= flags; 
    }

    uint64_t* strct = (uint64_t*)PDP_MAPPING(pml4_idx);

    if(!(strct[pdp_idx] & PAGE_FLAG_PRESENT)){
        strct[pdp_idx] = falloc() | PAGE_FLAG_PRESENT | flags;
        memset(PD_MAPPING(pml4_idx, pdp_idx), 0, 4096);
    }else{
        strct[pdp_idx] &= 0xFFFFFFFFFFFFFF001; 
        strct[pdp_idx] |= flags; 
    }

    strct = (uint64_t*)PD_MAPPING(pml4_idx, pdp_idx);

    if(!(strct[pd_idx] & PAGE_FLAG_PRESENT)){
        strct[pd_idx] = falloc() | PAGE_FLAG_PRESENT | flags;
        memset(PT_MAPPING(pml4_idx, pdp_idx, pd_idx), 0, 4096);
    }else{
        strct[pd_idx] &= 0xFFFFFFFFFFFFFF001; 
        strct[pd_idx] |= flags; 
    }

    strct = (uint64_t*)PT_MAPPING(pml4_idx, pdp_idx, pd_idx);
    strct[pt_idx] = physical | PAGE_FLAG_PRESENT | flags;
}

void unmap(uint64_t virtual){
    
    uint32_t v_hi     = virtual  >> 32;
    uint32_t v_lo     = (uint32_t)virtual;

    uint16_t pml4_idx =  (v_hi >> 7) & 0x1FF;
    uint16_t pdp_idx  =  ((v_hi<< 2) & 0x1FC) | ((v_lo >> 30) & 0x3);
    uint16_t pd_idx   =  (v_lo >> 21) & 0x1FF;
    uint16_t pt_idx   =  (v_lo >> 12) & 0x1FF;

    if(!(current_pml4[pml4_idx] & PAGE_FLAG_PRESENT)){
        return;
    }

    uint64_t* strct = (uint64_t*)PDP_MAPPING(pml4_idx);

    if(!(strct[pdp_idx] & PAGE_FLAG_PRESENT)){
        return;
    }

    strct = (uint64_t*)PD_MAPPING(pml4_idx, pdp_idx);

    if(!(strct[pd_idx] & PAGE_FLAG_PRESENT)){
        return;
    }

    strct = (uint64_t*)PT_MAPPING(pml4_idx, pdp_idx, pd_idx);
    strct[pt_idx] = 0x0;
}

uint64_t to_physical(uint64_t virtual){
    uint32_t v_hi     = virtual  >> 32;
    uint32_t v_lo     = (uint32_t)virtual;

    uint16_t pml4_idx =  (v_hi >> 7) & 0x1FF;
    uint16_t pdp_idx  =  ((v_hi<< 2) & 0x1FC) | ((v_lo >> 30) & 0x3);
    uint16_t pd_idx   =  (v_lo >> 21) & 0x1FF;
    uint16_t pt_idx   =  (v_lo >> 12) & 0x1FF;

    if(!(current_pml4[pml4_idx] & PAGE_FLAG_PRESENT)){
        return 0x0;
    }

    uint64_t* strct = (uint64_t*)PDP_MAPPING(pml4_idx);

    if(!(strct[pdp_idx] & PAGE_FLAG_PRESENT)){
        return 0x0;
    }

    strct = (uint64_t*)PD_MAPPING(pml4_idx, pdp_idx);

    if(!(strct[pd_idx] & PAGE_FLAG_PRESENT)){
        return 0x0;
    }

    strct = (uint64_t*)PT_MAPPING(pml4_idx, pdp_idx, pd_idx);
    if(!(strct[pt_idx] & PAGE_FLAG_PRESENT)){
        return 0x0;
    }

    return strct[pt_idx] & 0xFFFFF000;
}

uint64_t* copy_address_space(uint64_t npml4){
    uint64_t* new_pml4 = kvalloc(4096,4096);
    memcpy(new_pml4, npml4, 4096);
    return new_pml4;
}

void set_address_space(uint64_t npml4, uint8_t physical){
    if(physical){
        set_pml(npml4);
    }else{
        set_pml(to_physical(npml4));
        current_pml4 = (uint64_t)npml4;
    }
}

uint64_t  get_kernel_address_space(){
    return (uint64_t)pml4;
}
uint64_t  get_current_address_space(){
    return (uint64_t)current_pml4;
}

void set_mem_flags(uint64_t rg_start, uint64_t size, uint16_t flags){
    for(rg_start;rg_start<=rg_start+size;rg_start += 4096){
        mapf(to_physical(rg_start), rg_start,  flags);
    }
}