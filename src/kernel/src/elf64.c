#include <elf64.h>
#include <dev/log.h>
#include <string.h>
#include <memory.h>
#include <module.h>

static inline Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr)
{
    return (Elf64_Shdr *)((uint64_t)hdr + hdr->e_shoff);
}

static inline Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx)
{
    return &elf_sheader(hdr)[idx];
}

static inline char *elf_str_table(Elf64_Ehdr *hdr)
{
    if (hdr->e_shstrndx == SHN_UNDEF)
        return NULL;
    return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}

static inline char *elf_lookup_string(Elf64_Ehdr *hdr, int offset)
{
    char *strtab = elf_str_table(hdr);
    if (strtab == NULL)
        return NULL;
    return strtab + offset;
}

Elf64_Sym *elf_get_symbol(Elf64_Ehdr *target, const char *sname)
{
    Elf64_Shdr *sym_shdr = 0;
    char *shstrtab = elf_str_table(target);
    char *symstrtab = 0;
    uint16_t i = 0;
    for (unsigned int x = 0; x < target->e_shentsize * target->e_shnum; x += target->e_shentsize)
    {
        Elf64_Shdr *shdr = (Elf64_Shdr *)((uint64_t)target + (target->e_shoff + x));
        if (shdr->sh_type == SHT_SYMTAB)
        {
            sym_shdr = shdr;
        }
        if (shdr->sh_type == SHT_STRTAB && (!strcmp((char *)((uint64_t)shstrtab + shdr->sh_name), ".strtab")))
        {
            symstrtab = (char *)((uint64_t)target + shdr->sh_offset);
        }
    }
    if (!sym_shdr || !shstrtab || !symstrtab)
    {
        return 0;
    }
    Elf64_Sym *table = (Elf64_Sym *)((uint64_t)target + sym_shdr->sh_offset);
    while ((uint64_t)table - ((uint64_t)target + sym_shdr->sh_offset) < sym_shdr->sh_size)
    {
        if (table->st_name)
        {
            char *name = (char *)((uint64_t)symstrtab + table->st_name);
            if (!strcmp(name, sname))
            {
                return table;
            }
        }
        table++;
    }
    return 0;
}

void *elf_get_address(Elf64_Ehdr *hdr, Elf64_Sym *sym)
{
    Elf64_Shdr *target = elf_section(hdr, sym->st_shndx);
    return (uint64_t)hdr + target->sh_offset + sym->st_value;
}

int elf_get_symval(Elf64_Ehdr *hdr, int table, uint32_t idx)
{
    if (table == SHN_UNDEF || idx == SHN_UNDEF)
        return 0;

    Elf64_Shdr *symtab = elf_section(hdr, table);

    uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
    if (idx >= symtab_entries)
    {
        error("Symbol Index out of Range (%d:%u).", table, idx);
        return ELF_RELOC_ERR;
    }
    int symaddr = (int)hdr + symtab->sh_offset;
    Elf64_Sym *symbol = &((Elf64_Sym *)symaddr)[idx];
    if (symbol->st_shndx == SHN_UNDEF)
    {

        // External symbol, lookup value
        Elf64_Shdr *strtab = elf_section(hdr, symtab->sh_link);
        const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;

        void *target = get_kernel_symbol(name);

        if (target == NULL)
        {
            error("Undefined External Symbol : %s.", name);
            return ELF_RELOC_ERR;
        }
        else
        {
            return (int)target;
        }
    }
    else if (symbol->st_shndx == SHN_ABS)
    {
        // Absolute symbol
        return symbol->st_value;
    }
    else
    {
        // Internally defined symbol
        Elf64_Shdr *target = elf_section(hdr, symbol->st_shndx);
        return (int)hdr + symbol->st_value + target->sh_offset;
    }
}

static int elf_load_stage1(Elf64_Ehdr *hdr)
{
    Elf64_Shdr *shdr = elf_sheader(hdr);
    unsigned int i;
    // Iterate over section headers
    for (i = 0; i < hdr->e_shnum; i++)
    {
        Elf64_Shdr *section = &shdr[i];
        // If the section isn't present in the file
        if (section->sh_type == SHT_NOBITS)
        {
            // Skip if it the section is empty
            if (!section->sh_size)
                continue;
            // If the section should appear in memory
            if (section->sh_flags & SHF_ALLOC)
            {
                // Allocate and zero some memory
                void *mem = kmalloc(section->sh_size);
                memset(mem, 0, section->sh_size);
                // Assign the memory offset to the section offset
                section->sh_offset = (int)mem - (int)hdr;
                debug("Allocated memory for a section (%ld).", section->sh_size);
            }
        }
    }
    return 0;
}

static int elf_do_reloc(Elf64_Ehdr *hdr, Elf64_Rel *rel, Elf64_Shdr *reltab)
{

    Elf64_Shdr *target = elf_section(hdr, reltab->sh_info);

    int addr = (int)hdr + target->sh_offset;
    int *ref = (int *)(addr + rel->r_offset);
    // Symbol value
    int symval = 0;
    if (ELF64_R_SYM(rel->r_info) != SHN_UNDEF)
    {
        symval = elf_get_symval(hdr, reltab->sh_link, ELF64_R_SYM(rel->r_info));
        if (symval == ELF_RELOC_ERR)
            return ELF_RELOC_ERR;
    }
    // Relocate based on type
    switch (ELF64_R_TYPE(rel->r_info))
    {
    case R_X86_64_NONE:
        // No relocation
        break;
    case R_X86_64_32:
    case R_X86_64_32S:
    case R_X86_64_64:
        // Symbol + Offset
        *ref = DO_386_32(symval, *ref);
        break;
    case R_X86_64_PLT32:
    case R_X86_64_PC32:
        // Symbol + Offset - Section Offset
        *ref = DO_386_PC32(symval, *ref, (int)ref);
        break;
    default:
        // Relocation type not supported, display error and return
        error("Unsupported Relocation Type (%d).", ELF64_R_TYPE(rel->r_info));
        return ELF_RELOC_ERR;
    }
    return symval;
}

static int elf_do_reloca(Elf64_Ehdr *hdr, Elf64_Rela *rel, Elf64_Shdr *reltab)
{

    Elf64_Shdr *target = elf_section(hdr, reltab->sh_info);

    int addr = (int)hdr + target->sh_offset;
    int *ref = (int *)(addr + rel->r_offset);
    // Symbol value
    int symval = 0;
    if (ELF64_R_SYM(rel->r_info) != SHN_UNDEF)
    {
        symval = elf_get_symval(hdr, reltab->sh_link, ELF64_R_SYM(rel->r_info));
        if (symval == ELF_RELOC_ERR)
            return ELF_RELOC_ERR;
    }
    // Relocate based on type
    switch (ELF64_R_TYPE(rel->r_info))
    {
    case R_X86_64_NONE:
        // No relocation
        break;
    case R_X86_64_32:
    case R_X86_64_32S:
    case R_X86_64_64:
        // Symbol + Offset
        *ref = DO_386_32(symval, *ref) + rel->r_addend;
        break;
    case R_X86_64_PLT32:
    case R_X86_64_PC32:
        // Symbol + Offset - Section Offset
        *ref = DO_386_PC32(symval, *ref, (int)ref) + rel->r_addend;
        break;
    default:
        // Relocation type not supported, display error and return
        error("Unsupported Relocation Type (%d).", ELF64_R_TYPE(rel->r_info));
        return ELF_RELOC_ERR;
    }
    return symval;
}

static int elf_load_stage2(Elf64_Ehdr *hdr)
{
    Elf64_Shdr *shdr = elf_sheader(hdr);
    unsigned int i, idx;
    // Iterate over section headers
    for (i = 0; i < hdr->e_shnum; i++)
    {
        Elf64_Shdr *section = &shdr[i];
        if (section->sh_type == SHT_REL)
        {
            for (idx = 0; idx < section->sh_size / section->sh_entsize; idx++)
            {
                Elf64_Rel *reltab = &((Elf64_Rel *)((int)hdr + section->sh_offset))[idx];
                int result = elf_do_reloc(hdr, reltab, section);
                if (result == ELF_RELOC_ERR)
                {
                    error("Failed to relocate symbol.");
                    return ELF_RELOC_ERR;
                }
            }
        }
        else if (section->sh_type == SHT_RELA)
        {
            for (idx = 0; idx < section->sh_size / section->sh_entsize; idx++)
            {
                Elf64_Rela *reltab = &((Elf64_Rela *)((int)hdr + section->sh_offset))[idx];
                int result = elf_do_reloca(hdr, reltab, section);
                if (result == ELF_RELOC_ERR)
                {
                    error("Failed to relocate symbol.");
                    return ELF_RELOC_ERR;
                }
            }
        }
    }
    return 0;
}

uint64_t load_elf64(uint64_t start)
{
    info("Loading elf from 0x%x", start);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)start;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3 ||
        ehdr->e_ident[EI_CLASS] != ELFCLASS64)
    {
        error("Tried to parse invalid elf64 executable! 0x%x %c %c %c %d", ehdr->e_ident[EI_MAG0], ehdr->e_ident[EI_MAG1], ehdr->e_ident[EI_MAG2], ehdr->e_ident[EI_MAG3], ehdr->e_ident[EI_CLASS]);
        return 1;
    }
    if (ehdr->e_type == ET_EXEC)
    {
        debug("Loading elf executable");
        for (int i = 0; i < ehdr->e_phnum + 1; i++)
        {
            Elf64_Phdr *phdr = (Elf64_Phdr *)(start + ehdr->e_phoff + ehdr->e_phentsize * i);
            if (phdr->p_type == 1)
            {
                memset((void *)phdr->p_vaddr, 0, phdr->p_memsz);
                memcpy((void *)phdr->p_vaddr, (void *)(start + phdr->p_offset), phdr->p_filesz);
                debug("Loaded segment: 0x%llx - 0x%llx", phdr->p_vaddr, phdr->p_vaddr + phdr->p_memsz);
            }
        }
        return ehdr->e_entry;
    }
    else if (ehdr->e_type == ET_REL)
    {
        debug("Loading elf relocatable");
        int result;
        result = elf_load_stage1(ehdr);
        if (result == ELF_RELOC_ERR)
        {
            error("Unable to load ELF file.");
            return 1;
        }
        result = elf_load_stage2(ehdr);
        if (result == ELF_RELOC_ERR)
        {
            error("Unable to load ELF file.");
            return 1;
        }
        return 0;
    }
    else
    {
        error("Unknown elf type!");
        return 1;
    }
}