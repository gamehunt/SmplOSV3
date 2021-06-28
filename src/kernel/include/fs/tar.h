#ifndef __K_SMPLOS_TAR_H
#define __K_SMPLOS_TAR_H

#include <util.h>
#include <stdint.h>

typedef struct
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
} tar_header_t;

struct tar_file
{
    char filename[100];
    uint64_t start;
    uint32_t size;
};
typedef struct tar_file tar_file_t;

typedef struct{
    uint32_t entry_count;
    tar_file_t** entries;
} tar_parsed_t;

CH_START

tar_parsed_t* tar_parse(uint64_t);
uint32_t     tar_get_size(const char*);

CH_END

#endif