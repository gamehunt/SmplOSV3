global _start

_start:
    extern kernel_main
    call kernel_main
    hlt
    loop:jmp loop