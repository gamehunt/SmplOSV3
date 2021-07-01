#include <panic.h>
#include <dev/log.h>

extern uint64_t get_cr1();
extern uint64_t get_cr2();
extern uint64_t get_cr3();
extern uint64_t get_cr4();

void panic(regs_t dump, const char* message){
    asm("cli");
    error("--------------------------------------------------------------------");
    error("Unrecoverable error occurred in primary kernel and it was terminated.");
    error("Error description: %s", message);
    error("Error code: 0x%x", dump ? dump->err_code : 0);
    error("Registers dump:");
    if(dump){
        error("\tRAX: 0x%llx | RBX: 0x%llx | RCX: 0x%llx | RDX: 0x%llx", dump->rax, dump->rbx, dump->rcx, dump->rdx);
        error("\tRDI: 0x%llx | RSI: 0x%llx | RSP: 0x%llx | RBP: 0x%llx", dump->rdi, dump->rsi, dump->userrsp, dump->rbp);
        error("\tR8:  0x%llx | R9:  0x%llx | R10: 0x%llx | R11: 0x%llx", dump->r8, dump->r9, dump->r10, dump->r11);
        error("\tR12: 0x%llx | R13: 0x%llx | R14: 0x%llx | R15: 0x%llx", dump->r12, dump->r13, dump->r14, dump->r15);
        error("\tRIP: 0x%llx | RFLAGS: 0x%llx", dump->rip, dump->rflags);
        error("\tCR2: 0x%llx | CR3: 0x%llx", get_cr2(), get_cr3());
    }else{
        error("\t<Not available>");
    }
    error("--------------------------------------------------------------------");
    asm("hlt");
}