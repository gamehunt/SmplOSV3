#ifndef _BOOTLOADER_BOOTINFO_H
#define _BOOTLOADER_BOOTINFO_H

#ifdef _SMPLOS_BOOTLOADER
    #include <commons.h>
#else
    #include <boot/commons.h>
#endif

typedef struct{
    uint8_t dummy;
}bootinfo_t;

#endif