#include <proc.h>
#include <dev/log.h>
#include <elf64.h>
#include <memory.h>

static tss_entry_t* tss;
extern void      enter_userspace(void *, void *);

static proc_t* current_process = 0;
static proc_t* processes[256];
static uint32_t cpid = 0;
static uint32_t process_count = 0;

void schedule(proc_t* proc, regs_t regs){
    memcpy(current_process->context, regs, sizeof(struct registers));
    current_process = proc;
    set_address_space(current_process->address_space, 0);
    set_kernel_stack(current_process->kernel_stack);
    memcpy(regs, current_process->context, sizeof(struct registers));
}

void exec(uint64_t exec_start, uint8_t init){
    uint64_t entry = load_elf64(exec_start);
    if(CHECK_ADDR(entry)){
        proc_t* proc        = kmalloc(sizeof(proc_t));
        proc->pid           = 0;
        proc->name          = "init";
        proc->address_space = (uint64_t)copy_address_space(get_kernel_address_space());
        proc->kernel_stack  = (uint64_t)kvalloc(KERNEL_STACK_SIZE, 4096) + KERNEL_STACK_SIZE; 
        proc->stack         = USR_STACK + USR_STACK_SIZE;
        proc->context       = kmalloc(sizeof(struct registers));
        uint64_t prev_space = get_current_address_space();
        set_address_space(proc->address_space, 0);
        kalloc_f(USR_STACK, USR_STACK_SIZE/4096, 0, PAGE_FLAG_WRITABLE | PAGE_FLAG_USER);
        set_address_space(prev_space, 0);
        if(init){
            set_kernel_stack(proc->kernel_stack);
            set_address_space(proc->address_space, 0);

            processes[process_count] = proc;
            current_process = proc;
            process_count++;
            
            enter_userspace(entry, proc->stack);
        }
    }
}

void irq0_listener(regs_t regs){
    if(!process_count){
        return;
    }
    schedule(processes[cpid], regs);
    cpid++;
    if(cpid >= process_count){
        cpid = 0;
    }
}

void set_tss(tss_entry_t* t){
    tss = t;
}
void set_kernel_stack(uint64_t stack){
    tss->rsp0 = stack;
}