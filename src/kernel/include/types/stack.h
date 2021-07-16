#ifndef K_SMPLOS_STACK_H
#define K_SMPLOS_STACK_H

#include <stdint.h>

#define DEFINE_STACK(type, name)                      \
    typedef struct stack_##name{               \
        uint32_t   esp;                               \
        type*      stack;                             \
    }stack_##name##_t;                       \
    stack_##name##_t name;                   \
    static type __##name##_stack_size_helper;                      

#define INIT_STACK(name, size)  \
    name.esp = 0;               \
    name.stack = kmalloc(sizeof(__##name##_stack_size_helper)*size);

#define STACK_PUSH(name, val)         \
    name.stack[name.esp] = val; \
    name.esp++;

#define STACK_POP(name)           \
    name.stack[name.esp-1]; \
    name.esp--;

#define STACK_IS_EMPTY(name) \
    name.esp == 0

#define STACK_PUSH_RAW(ptr, type, value) \
    ptr -= sizeof(type);           \
	*((type*)ptr) = value;

#define STACK_POP_RAW(ptr, type)  \
    *((type*)ptr);          \
    ptr += sizeof(type);   

#endif