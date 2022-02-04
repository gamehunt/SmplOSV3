#ifndef _BOOTLOADER_MEM_H
#define _BOOTLOADER_MEM_H

#ifndef _SMPLOS_BOOTLOADER
    #error "Dont use this header for anything, that is not bootloader"
#endif

#include <commons.h>

void b_set_pml(uint64_t addr);
uint64_t* b_get_pml();

void b_setup_paging();

#endif