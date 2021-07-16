#include <syscall.h>

syscall_t syscalls[256];

uint64_t test_syscall()
{
    info("SYSCALL");
    return 0xAABBCCDD;
}

void setup_syscalls()
{
    syscalls[0] = test_syscall;
}

syscall_t get_syscall(uint64_t id)
{
    return syscalls[id];
}