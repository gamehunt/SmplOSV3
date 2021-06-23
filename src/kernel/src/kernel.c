#include <dev/log.h>
#include <interrupts.h>

void kernel_main(){
    info("Reached kernel entrypoint");
    setup_interrupts();
    for(;;) {
        asm("hlt");
    }
}

void _start(){
    kernel_main();
}