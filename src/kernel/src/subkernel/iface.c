#include <subkernel/iface.h>

static resource_dispatcher_t* dispatchers[256];
static uint8_t idx_ptr = 0;

uint8_t register_dispatcher(resource_dispatcher_t* dispatcher){
    dispatchers[idx_ptr] = dispatcher;
    dispatcher->idx = idx_ptr;
    idx_ptr++;
    info("Registered dispatcher with type 0x%x", dispatcher->type);
    return idx_ptr - 1;
}

resource_dispatcher_t* get_dispatcher(uint8_t idx){
    return dispatchers[idx];
}

resource_dispatcher_t* get_dispatcher_by_type(uint8_t type){
    for(uint8_t i=0;i<idx_ptr;i++){
        if(dispatchers[i] == type){
            return dispatchers[i];
        }
    }
    return 0;
}