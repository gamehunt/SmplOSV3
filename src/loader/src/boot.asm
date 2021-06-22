MAGIC         equ 0xe85250d6
ARCH          equ 0
HEADER_LENGTH equ header_end - header_start
CHECKSUM      equ  -(MAGIC + HEADER_LENGTH + ARCH)

WIDTH         equ 1024
HEIGHT        equ 768
DEPTH         equ 32

section .multiboot
align 8
header_start:
	dd MAGIC
	dd ARCH
	dd HEADER_LENGTH
    dd CHECKSUM
    dw 5
    dw 1
    dd 20
    dd WIDTH
    dd HEIGHT
    dd DEPTH
	dw 0
	dw 0
	dd 0 ;grub fucked up
header_end:

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
section .text
global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
 
    push eax
    push ebx

	extern loader_main
	call loader_main

	cli
.hang:	
	hlt
	jmp .hang
.end: