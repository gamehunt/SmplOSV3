global f
f:
    mov rax, 0
    int 0x7F
loop:
    jmp loop
