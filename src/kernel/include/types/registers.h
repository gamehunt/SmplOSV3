#ifndef K_SMPLOS_REGISTERS_H
#define K_SMPLOS_REGISTERS_H

#include <stdint.h>

struct registers{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rsp, rdi, rsi, rbp, rdx, rcx, rbx, rax;
	uint64_t int_no, err_code;
	uint64_t rip, cs, rflags, userrsp, ss;  
}__attribute__((packed));
typedef struct registers* regs_t;

#endif