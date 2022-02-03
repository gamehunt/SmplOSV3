#ifndef _BOOTLOADER_COMMONS_H
#define _BOOTLOADER_COMMONS_H

#ifdef _SMPLOS_BOOTLOADER
    #include <uefi.h>
#else
    #include <stdint.h>
#endif

typedef uint8_t BSTATUS;
typedef uint8_t BLOGLEVEL; 

#endif