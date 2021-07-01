#include <module.h>
#include <dev/log.h>
#include <elf64.h>
#include <memory.h>
#include <panic.h>
#include <subkernel/iface.h>
#include <io.h>
#include <string.h>


static uint16_t k_sym_ptr = 0;
static k_sym_t k_symtable[256];

static module_header_t* modules[256];
static uint8_t modules_ptr = 0;

static const char* required_modules[] = {
    "ata",
};

static uint8_t required_modules_left = REQUIRED_MODULES_TOTAL;

extern void putchar(char c);

void export_symbols(){
    export_symbol("putchar", &putchar);

    export_symbol("info"   , &info);
    export_symbol("warning", &warning);
    export_symbol("error"  , &error);
    export_symbol("debug"  , &debug);
    export_symbol("log"    , &log);

    export_symbol("outb", &outb);
    export_symbol("outw", &outw);
    export_symbol("outl", &outl);

    export_symbol("inb", &inb);
    export_symbol("inw", &inw);
    export_symbol("inl", &inl);

    export_symbol("kmalloc", &kmalloc);
    export_symbol("kfree", &kfree);

    export_symbol("register_dispatcher", &register_dispatcher);
}

uint8_t load_module(uint64_t start)
{
    if (!load_elf64(start))
    {
        Elf64_Ehdr *hdr = start;
        module_header_t *mhdr = elf_get_address(hdr, elf_get_symbol(hdr, "__k_module_header"));
        if(mhdr || mhdr->magic != SMPLOS_MODULE_MAGIC){
            if(!strlen(mhdr->name)){
                error("Module's name is empty! Skipping...");
            }else{
            info("Loading module '%s'...", mhdr->name);
                if(!mhdr->load()){
                    modules[modules_ptr] = mhdr;
                    modules_ptr++;

                    if(required_modules_left){
                        for(int i=0;i<REQUIRED_MODULES_TOTAL;i++){
                            if(strlen(required_modules[i]) && !strcmp(mhdr->name, required_modules[i])){
                                required_modules[i] = "";
                                required_modules_left--;
                            }
                        }
                    }

                }
            }
        }else{
            error("Failed to load module: header not found or invalid!");
            return 1;
        }
    }else{
        error("Failed to load module: faulty elf");
        return 1;
    }
}

uint8_t check_required_modules(){
    return required_modules_left == 0;
}

const char** get_required_modules_left(){
    return required_modules;
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