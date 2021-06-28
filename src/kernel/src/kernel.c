#include <dev/log.h>
#include <util.h>
#include <interrupts.h>
#include <loader/bootinfo.h>
#include <memory.h>

extern void      enter_userspace(void *, void *);
extern uint64_t  sysc();

uint64_t stack[1024];

void gpf(){
    sysc();
    while(1){;}
}

void kernel_main(bootinfo_t* bootinfo){
    info("Reached kernel entrypoint, bootinfo=0x%llx (%d entries, tss=0x%llx)", bootinfo, bootinfo->mem_size, bootinfo->tss);
    setup_interrupts();
    info("Initializing MMU");
    for(uint8_t i = 0;i< bootinfo->mem_size; i++){
        uint64_t start,end;
        uint8_t type;
        start = (((bootinfo_mem_region_t*)bootinfo->memory)[i]).start;
        end   = (((bootinfo_mem_region_t*)bootinfo->memory)[i]).end;
        type  = (((bootinfo_mem_region_t*)bootinfo->memory)[i]).type;
        info("0x%llx - 0x%llx - %x", start, end, type);
        if(type == 1){
            for(uint64_t addr = start; addr <= end; addr+= 4096){
                if(addr > 0x13ff000){
                    pmm_mark_frame(addr);
                }
            }
        }
    }
    info("PMM stack size = %d KB (end at 0x%llx)", pmm_get_stack_size()*8/1024, PMM_STACK_ADDRESS + pmm_get_stack_size()*8);
    init_paging();

    init_heap(MB(16));

    info("Mapping framebuffer at 0x%llx", bootinfo->framebuffer);

    for(uint32_t i=0;i<1024*768*32;i+=4096){
        map(bootinfo->framebuffer + i, bootinfo->framebuffer + i);
    }

    tss_entry_t* tss = (tss_entry_t*)bootinfo->tss;
    info("Kernel stack at %llx", tss->rsp0);

    enter_userspace(gpf, stack);

    for(;;) {
        asm("hlt");
    }
}

void _start(bootinfo_t* bootinfo){
    kernel_main(bootinfo);
}