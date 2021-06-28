#include <stdint.h>
#include <kernel/module.h>
#include <kernel/dev/log.h>
#include <stdio.h>

uint8_t load();
uint8_t unload();

SMPLOS_MODULE("test", load, unload)

uint8_t unload(){
    return 0;
}

uint8_t load(){
    printf("printf()\n\r");
    info("Module loaded!");
    return 0;
}  