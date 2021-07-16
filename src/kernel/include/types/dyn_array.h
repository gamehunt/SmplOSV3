#ifndef K_SMPLOS_DYN_ARRAY_H
#define K_SMPLOS_DYN_ARRAY_H

#include <stdint.h>

#define DEFINE_DYN_ARRAY(type, name)              \
    typedef struct dyn_array_##name{              \
        uint32_t   size;                          \
        uint32_t   tail;                          \
        type*      array;                         \
    }dyn_array_##name##_t;                        \
    dyn_array_##name##_t name;                    \
    static type __##name##_dyn_array_size_helper;    

#define INIT_DYN_ARRAY(name, _size)       \
    name.size  = _size;                   \
    name.tail  = 0;                       \
    name.array = kmalloc(sizeof(__##name##_dyn_array_size_helper)*_size);

#define DYN_ARRAY_GET(name, idx) \
    (name.array[idx])          

#define DYN_ARRAY_SET(name, idx, value) \
    name.array[idx] = value

#define DYN_ARRAY_ADD(name, value)        \
    if(name.tail + 1 >= name.size){       \ 
        name.size *= 2;                   \
        name.array = krealloc(name.array, name.size); \
    }                                     \
    name.array[name.tail] = value;        \
    name.tail++;    

#define DYN_ARRAY_SIZE(name)   \
    (name.tail)                

#endif