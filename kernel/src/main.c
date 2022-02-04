#include <boot/bootinfo.h>
#include <shared.h>

void kernel_main(bootinfo_t* info){
    
    UNUSED(info);

    __asm__("movq $0x1337, %rax");

    while(1);
}