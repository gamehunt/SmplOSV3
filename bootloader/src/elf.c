#include <elf.h>
#include <log.h>
#include <mem.h>

static BSTATUS __check_hdr(Elf64_Ehdr* elf){
    if(memcmp(elf->e_ident, ELFMAG, SELFMAG) || 
        elf->e_ident[EI_CLASS] != ELFCLASS64 || 
        elf->e_ident[EI_DATA] != ELFDATA2LSB || 
        elf->e_type != ET_EXEC               || 
        elf->e_machine != EM_X86_64          ||
       !elf->e_phnum
        ){
        return 1;
    }
    return 0;
}

BSTATUS load_elf(const char* path, uintptr_t* entry){

    FILE *f;
    char *buff;
    long int size;

    if((f = fopen(path, "r"))) {
        fseek(f, 0, SEEK_END);
        size = ftell(f);

        fseek(f, 0, SEEK_SET);
        buff = malloc(size + 1);

        if(!buff) {
            return ELF_ERR_GENERIC;
        }

        fread(buff, size, 1, f);
        fclose(f);

        Elf64_Ehdr* elf = (Elf64_Ehdr*) buff;

        if(__check_hdr(elf)){
            return ELF_ERR_INVALID_FORMAT;
        }

        Elf64_Phdr *phdr;
        int i;
        for(phdr = (Elf64_Phdr *)(buff + elf->e_phoff), i = 0;
                i < elf->e_phnum;
                i++, phdr = (Elf64_Phdr *)((uint8_t *)phdr + elf->e_phentsize)) {
                    if(phdr->p_type == PT_LOAD) {
                        b_map_range(phdr->p_vaddr, phdr->p_memsz / 0x1000 + (phdr->p_memsz % 0x1000 > 0 ? 1 : 0), 0);
                        memcpy((void*)phdr->p_vaddr, buff + phdr->p_offset, phdr->p_filesz); 
                        memset((void*)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
                    }
        }
        *entry = elf->e_entry;
        free(buff);
        return ELF_ERR_SUCCESS;
    } else {
        return ELF_ERR_READ_FAILED;
    }
}