void kernel_main(){
    asm("hlt");
}

void _start(){
    kernel_main();
}