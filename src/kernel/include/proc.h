#ifndef __K_SMPLOS_PROC_H
#define __K_SMPLOS_PROC_H

#include <util.h>
#include <types/registers.h>
#include <memory.h>
typedef struct proc{
    int32_t   pid;
    const char* name;
    uint64_t  address_space;
    uint64_t  stack;
    uint64_t  kernel_stack;
    regs_t    context;
}proc_t;

CH_START

void irq0_listener(regs_t regs);

void schedule(proc_t* proc, regs_t regs);

void exec(uint64_t exec_start, const char* name, uint8_t init);

void set_tss(tss_entry_t* tss);
void set_kernel_stack(uint64_t stack);

CH_END

#endif