#ifndef K_SMPLOS_STACK_H
#define K_SMPLOS_STACK_H

#include <stdint.h>

#define DEFINE_STACK(type, name)                      \
    typedef struct stack_##type_##name{               \
        uint32_t   esp;                               \
        type*      stack;                             \
    }stack_##type##_##name##_t;                       \
    stack_##type##_##name##_t name;                   \
    static type __##name##_stack_size_helper;                      

#define INIT_STACK(name, size)  \
    name.esp = 0;               \
    name.stack = kmalloc(sizeof(__##name##_stack_size_helper)*size);

#define PUSH(name, val)         \
    name.stack[name.esp] = val; \
    name.esp++;

#define POP(name)           \
    name.stack[name.esp-1]; \
    name.esp--;

#define IS_EMPTY(name) \
    name.esp == 0

#define PUSH_RAW(ptr, type, value) \
    ptr -= sizeof(type);           \
	*((type*)ptr) = value;

#define POP_RAW(ptr, type)  \
    *((type*)ptr);          \
    ptr += sizeof(type);   

#endif