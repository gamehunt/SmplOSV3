#include <memory.h>
#include <string.h>
#include <dev/log.h>

uint64_t* stack = (uint64_t*)PMM_STACK_ADDRESS;
uint32_t  esp = 0;
uint32_t  stack_size = 0;
uint32_t* metadata_bitmap;

uint64_t falloc(){
    uint64_t frame = stack[esp-1];
    esp--;
    return frame;
}

void pmm_mark_frame(uint64_t frame){
    stack[esp] = frame;
    esp += 1;
    stack_size += 1;
}

uint32_t pmm_get_stack_size(){
    return stack_size;
}

void pmm_init_metadata_bitmap(){
    metadata_bitmap = (uint32_t*) (PMM_STACK_ADDRESS + stack_size * 8);
    memset(metadata_bitmap, 0, sizeof(uint32_t)*stack_size);
    info("PMM metadata bitmap initialized at 0x%llx", metadata_bitmap);
}