#include <elf64.h>
#include <log.h>
#include <util/string.h>

uint32_t load_elf64(uint32_t start){
    info("Loading elf from 0x%x", start);
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)start;
    if(ehdr->e_ident[EI_MAG0]  != ELFMAG0 || 
       ehdr->e_ident[EI_MAG1]  != ELFMAG1 ||
       ehdr->e_ident[EI_MAG2]  != ELFMAG2 ||
       ehdr->e_ident[EI_MAG3]  != ELFMAG3 ||
       ehdr->e_ident[EI_CLASS] != ELFCLASS64
       ){
           error("Tried to parse invalid elf64 executable! 0x%x %c %c %c %d",ehdr->e_ident[EI_MAG0],ehdr->e_ident[EI_MAG1],ehdr->e_ident[EI_MAG2],ehdr->e_ident[EI_MAG3],ehdr->e_ident[EI_CLASS]);
           return;
    }
    for(int i=0;i<ehdr->e_phnum+1;i++){
        Elf64_Phdr* phdr = (Elf64_Phdr*)(start + ehdr->e_phoff + ehdr->e_phentsize*i);
        if(phdr->p_type == 1){
            memset((void*)phdr->p_vaddr,0, phdr->p_memsz);
            memcpy((void*)phdr->p_vaddr, (void*)(start + phdr->p_offset), phdr->p_filesz);
            info("Loaded segment: 0x%llx - 0x%llx", phdr->p_vaddr, phdr->p_vaddr + phdr->p_memsz);
        }
    }
    info("Trying entrypoint: 0x%llx", ehdr->e_entry);
    return ehdr->e_entry;
}