#ifndef __K_SMPLOS_SUBK_IFACE_H
#define __K_SMPLOS_SUBK_IFACE_H

#include <stdint.h>

#if defined(K_SMPLOS_KERNEL)
    #include <util.h>
#else
    #include <kernel/util.h>
#endif

#define RESOURCE_DISPATCHER_TYPE_DSK   0x0
#define RESOURCE_DISPATCHER_TYPE_NET   0x1
#define RESOURCE_DISPATCHER_TYPE_UND   0xFF

typedef struct{
    uint8_t idx;
    uint8_t type;
    void* (*alloc)(uint64_t size, void* meta);
    void* (*free) (void* ptr, void* meta);
    uint64_t (*read) (void* ptr,void* buffer, uint64_t offset, uint64_t size);
    uint64_t (*write)(void* ptr, void* buffer, uint64_t size);
}resource_dispatcher_t;

CH_START

uint8_t register_dispatcher(resource_dispatcher_t* dispatcher);
resource_dispatcher_t* get_dispatcher(uint8_t idx);
resource_dispatcher_t* get_dispatcher_by_type(uint8_t type);

CH_END

#endif