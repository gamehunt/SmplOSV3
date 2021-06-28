#include <module.h>
#include <dev/log.h>
#include <elf64.h>
#include <memory.h>
#include <panic.h>
#include <subkernel/iface.h>
#include <io.h>

static uint16_t k_sym_ptr = 0;
static k_sym_t k_symtable[256];

extern void putchar(char c);

void export_symbols(){
    export_symbol("putchar", &putchar);

    export_symbol("info", &info);
    export_symbol("warning", &warning);
    export_symbol("error", &error);
    export_symbol("debug", &debug);
    export_symbol("log", &log);

    export_symbol("outb", &outb);
    export_symbol("outw", &outw);
    export_symbol("outl", &outl);

    export_symbol("inb", &inb);
    export_symbol("inw", &inw);
    export_symbol("inl", &inl);

    export_symbol("set_disk_sector_dispatcher", &set_disk_sector_dispatcher);
}

uint8_t load_module(uint64_t start)
{
    if (!load_elf64(start))
    {
        Elf64_Ehdr *hdr = start;
        module_header_t *mhdr = elf_get_address(hdr, elf_get_symbol(hdr, "__k_module_header"));
        if(mhdr || mhdr->magic != SMPLOS_MODULE_MAGIC){
            info("Loading module '%s'...", mhdr->name);
            return mhdr->load();
        }else{
            error("Failed to load module: header not found or invalid!");
            return 1;
        }
    }else{
        error("Failed to load module: faulty elf");
        return 1;
    }
}

uint64_t get_kernel_symbol(const char* name){
    for(int i=0;i<k_sym_ptr;i++){
        if(!strcmp(k_symtable[i].name, name)){
            return k_symtable[i].address;
        }
    }
    return NULL;
}

void     export_symbol(const char* name, uint64_t address){
    if(k_sym_ptr >= 256){
        panic(NULL, "Symtable overflow");
        return;
    }
    k_symtable[k_sym_ptr].name = name;
    k_symtable[k_sym_ptr].address = address;
    k_sym_ptr++;
}