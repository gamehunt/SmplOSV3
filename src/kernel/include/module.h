#ifndef __K_SMPLOS_MODULE_H
#define __K_SMPLOS_MODULE_H

#include <stdint.h>

#define SMPLOS_MODULE_MAGIC 0xDEADCADE
typedef struct{
    uint32_t magic;
    const char* name;
    uint8_t (*load)(void);
    uint8_t (*unload)(void);
}module_header_t;

typedef struct{
    const char* name;
    uint64_t address;
}k_sym_t; //TODO dictionary type

#define SMPLOS_MODULE(name,load,unload) \
    module_header_t __k_module_header = {SMPLOS_MODULE_MAGIC, name, load, unload};

uint8_t load_module(uint64_t start);
uint64_t get_kernel_symbol(const char* name);
void     export_symbol(const char* name, uint64_t address);
void    export_symbols();

#endif