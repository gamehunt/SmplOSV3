#include <dev/log.h>
#include <util.h>
#include <interrupts.h>
#include <loader/bootinfo.h>
#include <memory.h>

void kernel_main(bootinfo_t* bootinfo){
    info("Reached kernel entrypoint, bootinfo=0x%llx (%d entries)", bootinfo, bootinfo->mem_size);
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

    for(;;) {
        asm("hlt");
    }
}

void _start(bootinfo_t* bootinfo){
    kernel_main(bootinfo);
}