global _start

_start:
    extern kernel_main
    call kernel_main
    loop:jmp loop