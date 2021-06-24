#include <panic.h>
#include <dev/log.h>

void panic(regs_t dump, const char* message){
    error("--------------------------------------------------------------------");
    error("Unrecoverable error occurred in primary kernel and it was terminated.");
    error("Error description: %s", message);
    error("Registers dump:");
    if(dump){
        printf("\tRAX: 0x%llx | RBX: 0x%llx | RCX: 0x%llx | RDX: 0x%llx\r\n", dump->rax, dump->rbx, dump->rcx, dump->rdx);
        printf("\tRDI: 0x%llx | RSI: 0x%llx | RSP: 0x%llx | RBP: 0x%llx\r\n", dump->rdi, dump->rsi, dump->userrsp, dump->rbp);
        printf("\tR8:  0x%llx | R9:  0x%llx | R10: 0x%llx | R11: 0x%llx\r\n", dump->r8, dump->r9, dump->r10, dump->r11);
        printf("\tR12: 0x%llx | R13: 0x%llx | R14: 0x%llx | R15: 0x%llx\r\n", dump->r12, dump->r13, dump->r14, dump->r15);
        printf("\tRIP: 0x%llx | RFLAGS: 0x%llx\r\n", dump->rip, dump->rflags);
    }else{
        printf("\t<Not available>\r\n");
    }
    error("--------------------------------------------------------------------");
    asm("hlt");
}