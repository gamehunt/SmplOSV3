#include <stdint.h>
#include <kernel/module.h>
#include <kernel/dev/log.h>
#include <stdio.h>
#include <kernel/subkernel/iface.h>

uint8_t load();
uint8_t unload();

SMPLOS_MODULE("ata", load, unload)

uint8_t unload(){
    return 0;
}

uint8_t load(){
    set_disk_sector_dispatcher(0);
    info("ATA module loaded");
    return 0;
}  