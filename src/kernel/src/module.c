#include <module.h>
#include <dev/log.h>
#include <dev/vfs.h>
#include <interrupts.h>
#include <elf64.h>
#include <memory.h>
#include <panic.h>
#include <io.h>
#include <string.h>
#include <types/dyn_array.h>


DEFINE_DYN_ARRAY(k_sym_t*, k_symtable)
DEFINE_DYN_ARRAY(module_header_t*, modules)

static const char* required_modules[] = {
    "ata",
};

static uint8_t required_modules_left = REQUIRED_MODULES_TOTAL;
static uint8_t __are_modules_initialized = 0;

extern void putchar(char c);
extern void* __libk_malloc(size_t);
extern void  __libk_free(void*);
extern void  __libk_assertion_fail(const char*, const char*, int);

void export_symbols(){

    INIT_DYN_ARRAY(k_symtable, 256);

    export_symbol("putchar", &putchar);

    export_symbol("__libk_malloc", &__libk_malloc);
    export_symbol("__libk_free",   &__libk_free);
    export_symbol("__libk_assertion_fail", &__libk_assertion_fail);

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

    export_symbol("set_irq_handler", &set_irq_handler);
    export_symbol("irq_end", &irq_end);

    export_symbol("kcreate", &kcreate);
    export_symbol("kread", &kread);
    export_symbol("kwrite", &kwrite);
    export_symbol("kmount", &kmount);

    export_symbol("vfs_print_tree", &vfs_print_tree);
}

uint8_t load_module(uint64_t start)
{
    if(!__are_modules_initialized){
        INIT_DYN_ARRAY(modules, 256);
        __are_modules_initialized = 1;
    }
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
                    DYN_ARRAY_ADD(modules, mhdr)
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
    for(int i=0;i<DYN_ARRAY_SIZE(k_symtable);i++){
        k_sym_t* sym = DYN_ARRAY_GET(k_symtable, i);
        if(!strcmp(sym->name, name)){
            return sym->address;
        }
    }
    return NULL;
}

void     export_symbol(const char* name, uint64_t address){
    k_sym_t* sym = kmalloc(sizeof(k_sym_t));
    sym->name = name;
    sym->address = address;
    DYN_ARRAY_ADD(k_symtable, sym);
}