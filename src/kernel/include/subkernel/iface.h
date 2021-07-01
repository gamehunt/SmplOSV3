#ifndef __K_SMPLOS_SUBK_IFACE_H
#define __K_SMPLOS_SUBK_IFACE_H

#include <stdint.h>

#if defined(K_SMPLOS_KERNEL)
    #include <util.h>
#else
    #include <kernel/util.h>
#endif

typedef struct{
    uint8_t idx;
    void* (*alloc)(uint64_t size, void* meta);
    void* (*free) (void* ptr, void* meta);
}resource_dispatcher_t;

CH_START

uint8_t register_dispatcher(resource_dispatcher_t* dispatcher);
resource_dispatcher_t* get_dispatcher(uint8_t idx);

CH_END

#endif