#include <subkernel/iface.h>

static resource_dispatcher_t* dispatchers[256];
static uint8_t idx_ptr = 0;

uint8_t register_dispatcher(resource_dispatcher_t* dispatcher){
    dispatchers[idx_ptr] = dispatcher;
    dispatcher->idx = idx_ptr;
    idx_ptr++;
}

resource_dispatcher_t* get_dispatcher(uint8_t idx){
    return dispatchers[idx];
}