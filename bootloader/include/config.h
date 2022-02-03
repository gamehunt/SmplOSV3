#ifndef _BOOTLOADER_CONFIG_H
#define _BOOTLOADER_CONFIG_H

#include <commons.h>

#define CFG_ERR_SUCCESS         0
#define CFG_ERR_READ_FAILED     1
#define CFG_ERR_PARSING_FAILED  2
#define CFG_ERR_INVALID_ARGS    3
#define CFG_ERR_GENERIC         255

struct config_node{
    char* name;
    char* value;
};

struct config_error{
    BSTATUS status;
    char* description;
};

struct config_result{
    struct config_error error;
    uint32_t child_count;
    struct config_node* childs;
};

typedef struct config_result  config_result_t;
typedef struct config_node  config_node_t;
typedef struct config_error config_error_t;

BSTATUS b_config_parse(const char* path, config_result_t* result);

#endif