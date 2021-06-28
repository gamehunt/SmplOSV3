#ifndef __K_SMPLOS_LOADER_BOOTINFO_H
#define __K_SMPLOS_LOADER_BOOTINFO_H

#include <memory.h>

struct bootinfo_mem_region{
    uint64_t start;
    uint64_t end;
    uint8_t  type;
} __attribute__((packed));

typedef struct bootinfo_mem_region bootinfo_mem_region_t;

struct bootinfo_module{
    uint64_t start;
    uint64_t size;
    char     cmd[256];
} __attribute__((packed));

typedef struct bootinfo_module bootinfo_module_t;

struct bootinfo
{
    bootinfo_mem_region_t memory[256];
    uint8_t mem_size;
    uint64_t framebuffer;
    uint64_t initrd;
    uint64_t tss;
    bootinfo_module_t modules[32];
} __attribute__((packed));

typedef struct bootinfo bootinfo_t;

#endif