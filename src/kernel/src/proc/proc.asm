global enter_userspace
enter_userspace:

    mov ax, 0x23 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
 
	push rax
    push rsi
	pushf
	push 0x1b ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push rdi  ; instruction address to return to
	iretq

;global sysc
;sysc:
;	mov rax, 0
;	int 0x7f
;	ret