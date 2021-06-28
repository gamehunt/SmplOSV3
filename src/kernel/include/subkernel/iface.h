#ifndef __K_SMPLOS_SUBK_IFACE_H
#define __K_SMPLOS_SUBK_IFACE_H

#include <stdint.h>

typedef struct{
    uint16_t sector_size;
}disk_sector_dispatcher_t;

void set_disk_sector_dispatcher(disk_sector_dispatcher_t* dispatcher);

#endif