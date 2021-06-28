#ifndef __K_SMPLOS_SYSCALL_H
#define __K_SMPLOS_SYSCALL_H

#include <util.h>
#include <types/registers.h>

typedef uint64_t(* syscall_t)(uint64_t a,uint64_t b,uint64_t c,uint64_t d,uint64_t e);

CH_START

void setup_syscalls();
syscall_t get_syscall(uint64_t id);
void syscall_handler(regs_t regs);

CH_END

#endif