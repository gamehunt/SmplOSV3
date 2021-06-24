
[BITS 64]
%macro pushAll 0
      push   rax
      push   rbx
      push   rcx
      push   rdx
      push   rbp
      push   rsi
      push   rdi
      push   rsp
      push   r8
      push   r9
      push   r10
      push   r11
      push   r12
      push   r13
      push   r14
      push   r15
%endmacro

%macro popAll 0
      pop       r15
      pop       r14
      pop       r13
      pop       r12
      pop       r11
      pop       r10
      pop       r9
      pop       r8
      pop       rsp
      pop       rdi
      pop       rsi
      pop       rbp
      pop       rdx
      pop       rcx
      pop       rbx
      pop       rax
%endmacro

global idt_load
extern idt_ptr

idt_load:							;loads the idt

cli
lidt[idt_ptr+2]	
sti
ret

%macro ISR_NOERR 1
	global isr%1
	isr%1:
	  cli
	  push 0
	  push %1
	  jmp isr_stub
%endmacro

%macro ISR_ERR 1
	global isr%1
	isr%1:
	  cli
	  push %1
	  jmp isr_stub
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 127

extern fault_handler

isr_stub:
    pushAll

    mov rdi, rsp 

    mov     ax, ds
    push    rax

    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    
    call fault_handler

    pop      rbx
    mov      ds, bx
    mov      es, bx
    mov      fs, bx
    mov      gs, bx
 
    popAll
    add rsp,16
    sti
    iretq

%macro IRQ 2
	global irq%1
	irq%1:
		cli
		push 0x00
		push %2
		jmp irq_stub
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

extern irq_handler

irq_stub:
    cli
    pushAll  ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    
    mov rdi, rsp 
                      
    mov ax, ds               ; Lower 16-bits of eax = ds.
    push rax                 ; save the data segment descriptor

    mov ax, 0x10  ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler
    
    pop rbx        ; reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popAll                     ; Pops edi,esi,ebp..

    add rsp, 16      ; Cleans up the pushed error code and pushed ISR number
    sti
    iretq           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP