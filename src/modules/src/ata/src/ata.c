#include <stdint.h>
#include <kernel/module.h>
#include <kernel/dev/log.h>
#include <stdio.h>
#include <kernel/subkernel/iface.h>

uint8_t load();
uint8_t unload();

SMPLOS_MODULE("ata", load, unload)

void* alloc(uint64_t size, void* meta){
    return kmalloc(1);
}

void* free(void* ptr, void* meta){
    return kmalloc(1);
}

uint64_t read(void* ptr,void* buffer, uint64_t offset, uint64_t size){
    return 0;
}

uint64_t write(void* ptr, void* buffer, uint64_t size){
    return 0;
}   

resource_dispatcher_t ata_dispatcher;

uint8_t unload(){
    return 0;
}

uint8_t load(){

    ata_dispatcher.type = RESOURCE_DISPATCHER_TYPE_DSK;

    ata_dispatcher.alloc = alloc;
    ata_dispatcher.free = free;
    ata_dispatcher.read = read;
    ata_dispatcher.write = write;

    uint8_t idx = register_dispatcher(&ata_dispatcher);
    info("ATA dispatcher registered with id %d", idx);
    info("ATA module loaded");
    return 0;
}  