#ifndef __K_SMPLOS_LOADER_BOOTINFO_H
#define __K_SMPLOS_LOADER_BOOTINFO_H

#include <memory.h>

struct bootinfo_mem_region{
    uint64_t start;
    uint64_t end;
    uint8_t  type;
} __attribute__((packed));

typedef struct bootinfo_mem_region bootinfo_mem_region_t;

struct bootinfo
{
    bootinfo_mem_region_t memory[256];
    uint8_t mem_size;
    uint64_t framebuffer;
    uint64_t tss;
} __attribute__((packed));

typedef struct bootinfo bootinfo_t;

#endif