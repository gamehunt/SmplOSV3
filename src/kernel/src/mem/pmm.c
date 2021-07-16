#include <memory.h>
#include <string.h>
#include <dev/log.h>

static uint64_t *stack = (uint64_t *)PMM_STACK_ADDRESS;
static uint32_t esp = 0;
static uint32_t stack_size = 0;

uint64_t falloc()
{
    uint64_t frame = stack[esp - 1];
    esp--;
    return frame;
}

void ffree(uint64_t frame)
{
    stack[esp] = frame;
    esp++;
    if (esp > stack_size)
    {
        stack_size = esp;
    }
}

void pmm_mark_frame(uint64_t frame)
{
    stack[esp] = frame;
    esp += 1;
    stack_size += 1;
}

uint32_t pmm_get_stack_size()
{
    return stack_size;
}