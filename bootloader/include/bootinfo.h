#ifndef _BOOTLOADER_BOOTINFO_H
#define _BOOTLOADER_BOOTINFO_H

#ifdef _SMPLOS_BOOTLOADER
    #include <commons.h>
#else
    #include <boot/commons.h>
#endif

#define B_MEMTYPE_RESERVED 0
#define B_MEMTYPE_ALLOWED  1
#define B_MEMTYPE_ACPI     2

typedef struct{
    uint64_t start;
    uint64_t size; //In pages
    uint8_t type;   
}memmap_entry_t;

typedef struct{
    memmap_entry_t* entries;
    uint32_t size;
}memmap_info_t;

typedef struct{
    uint64_t addr;
    uint64_t size;
    uint32_t width;
    uint32_t height;
    uint32_t pixels_per_scanline;
    uint32_t pixel_format;
}fb_info_t;

typedef struct{
    memmap_info_t* memmap;
    fb_info_t* framebuffer;
}bootinfo_t;

#endif