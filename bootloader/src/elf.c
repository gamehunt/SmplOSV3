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

BSTATUS load_elf(char* buff, uintptr_t* entry){
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
                        memcpy((void*)phdr->p_vaddr, buff + phdr->p_offset, phdr->p_filesz); 
                        memset((void*)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
                    }
    }
    *entry = elf->e_entry;
    return ELF_ERR_SUCCESS;
}