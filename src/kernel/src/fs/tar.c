#include <fs/tar.h>
#include <dev/log.h>
#include <memory.h>
#include <string.h>

tar_parsed_t* tar_parse(uint64_t addr){
    tar_file_t** entries = (tar_file_t**)kmalloc(sizeof(tar_file_t*)*64);
    uint32_t i;
    for (i = 0; ; i++)
    {
        tar_header_t *header = (tar_header_t *)addr;
        if (header->filename[0] == '\0')
            break;
        uint32_t size = tar_get_size(header->size);
        entries[i] = kmalloc(sizeof(tar_file_t));
        tar_file_t* file = entries[i];
        strcpy(file->filename, header->filename);
        file->size  = size;
        file->start = (uint64_t)((uint64_t)(header) + 512);
        addr+= ((size / 512) + 1) * 512;
        if (size % 512)
            addr += 512;
    }
    tar_parsed_t* parsed = kmalloc(sizeof(tar_parsed_t));
    parsed->entry_count = i;
    parsed->entries = entries;
    return parsed;
}

uint32_t tar_get_size(const char* in){
    uint32_t size = 0;
    uint32_t j;
    uint32_t count = 1;
    for (j = 11; j > 0; j--, count *= 8){
        size += ((in[j - 1] - '0') * count);
    }
    return size;
}