#ifndef _BOOTLOADER_UTILS_H
#define _BOOTLOADER_UTILS_H

#include <commons.h>

BSTATUS b_read_file(const char* path, char** buffer, uint32_t* size);

#endif