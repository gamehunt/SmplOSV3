global gdt_load
extern gp
extern pml4
extern paging_setup

gdt_load:
    cli         ;
    lgdt [gp+2]       
    mov eax,0x10
    mov ds,eax
    mov es,eax
    mov fs,eax
    mov gs,eax
    mov ss,eax
    ret

global setup_longmode

setup_longmode:
    ; disable paging
    mov eax,cr0
    and eax, 0x7FFFFFFF       ; b01111111111111111111111111111111
    mov cr0, eax

    ; setup paging
    call paging_setup
    mov eax, pml4
    mov cr3, eax

    ;hlt

    ; enable PAE
    mov eax,  cr4
    or  eax,  0x20            ; b00000000000000000000000000100000
    mov cr4,  eax

    
    ; goto IA32e
    mov   ecx, 0xC0000080
    rdmsr
    or    eax, 0x100          ; b00000000000000000000000100000000
    wrmsr

    ; enable paging
    mov eax,cr0
    or  eax, 0x80000000       ; b10000000000000000000000000000000
    mov cr0, eax
    
    
    ret

global enter_kernel

GDT64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).
    .Code: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .Pointer:                    ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64          

enter_kernel:
	push	ebp
	mov	    ebp, esp                ; Set up the stack so the variables passed from the C code can be read
 
	mov	esi, [ebp+8]                ; This is the kernel entry point
	mov	[k_ptr], esi
 
    
	lgdt	[GDT64.Pointer]                  ; Load GDT
    
	jmp	GDT64.Code:Realm64        ; Reload code selector by jumping to 64-bit code

[BITS 64]
Realm64:
    cli                           ; Clear the interrupt flag.
    mov ax, GDT64.Data            ; Set the A-register to the data descriptor.
    mov ds, ax                    ; Set the data segment to the A-register.
    mov es, ax                    ; Set the extra segment to the A-register.
    mov fs, ax                    ; Set the F-segment to the A-register.
    mov gs, ax                    ; Set the G-segment to the A-register.
    mov ss, ax                    ; Set the stack segment to the A-register.

	mov	edi, [ebp + 12]		    ; 1st argument of kernel_main (pointer to multiboot structure)
	mov	rax, [k_ptr] 
    push rax                                                                                                   ; This is transformed to mov rax, [k_ptr] and uses the double word reserved below 
	ret                     ; This part is plain bad, tricking the processor is not the best thing to do here
 
section .data
k_ptr:
	dd	0