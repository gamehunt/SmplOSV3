#if defined(__linux__)
#error "Cant find cross compiler"
#endif
 
#if !defined(__i386__)
#error "Invalid arch, need i686-elf"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <io.h>
#include <util/printf.h>
#include <util/string.h>
#include <multiboot2.h>
#include <log.h>
#include <memory.h>
#include <elf64.h>
#include <serial.h>
#include <bootinfo.h>

extern void setup_longmode();
extern void enter_kernel(uint32_t entry, uint32_t bootinfo);

extern tss_entry_t tss_entry;

bootinfo_t bootinfo;
bootinfo_mem_region_t mem_regions[256];

void loader_main(uint32_t mbt, uint32_t magic) 
{

    enable_a20();
    serial_init();

    info("Loading kernel");

    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC){
        error("Multiboot check failed!");
        return;
    }

    if (mbt & 7)
    {
        error ("Unaligned multiboot info: 0x%x\n", mbt);
        return;
    }

    gdt_install();

    struct multiboot_tag* tag;
    struct multiboot_tag_framebuffer *tagfb; 
    multiboot_memory_map_t *tag_mmap;
    uint32_t *fb;
    uint32_t kernel_start = 0xFFFFFFFF;
    uint32_t end = 0;
    uint8_t memreg_ptr = 0;

    for (
       tag = (struct multiboot_tag *) (mbt + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                info("Loaded with %s", ((struct multiboot_tag_string *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                tagfb = (struct multiboot_tag_framebuffer *) tag;
                fb = (uint32_t *) (unsigned long) tagfb->common.framebuffer_addr;
                info("Found framebuffer address: 0x%x", fb);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                info("Found memory map");
                for (tag_mmap = ((struct multiboot_tag_mmap *) tag)->entries;
                 (multiboot_uint8_t *) tag_mmap < (multiboot_uint8_t *) tag + tag->size;
                 tag_mmap = (multiboot_memory_map_t *) ((unsigned long) tag_mmap + ((struct multiboot_tag_mmap *) tag)->entry_size)) {
                     
                    bootinfo.memory[memreg_ptr].start = tag_mmap->addr;
                    bootinfo.memory[memreg_ptr].end   = tag_mmap->addr + tag_mmap->len;
                    bootinfo.memory[memreg_ptr].type  = tag_mmap->type;

                    memreg_ptr++;
                }
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                info ("Module at 0x%x-0x%x. Command line %s",
                  ((struct multiboot_tag_module *) tag)->mod_start,
                  ((struct multiboot_tag_module *) tag)->mod_end,
                  ((struct multiboot_tag_module *) tag)->cmdline);
                if(end < ((struct multiboot_tag_module *) tag)->mod_end){
                    end = ((struct multiboot_tag_module *) tag)->mod_end;
                }
                if(!strcmp(((struct multiboot_tag_module *) tag)->cmdline, "KERNEL")){
                    info ("Identified kernel");
                    kernel_start = ((struct multiboot_tag_module *) tag)->mod_start;
                }
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                info ("Found memory info: mem_lower = %uKB, mem_upper = %uKB",
                  ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
                  ((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
                break;
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                info ("Command line = %s",
                  ((struct multiboot_tag_string *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                info ("Boot device 0x%x,%u,%u",
                  ((struct multiboot_tag_bootdev *) tag)->biosdev,
                  ((struct multiboot_tag_bootdev *) tag)->slice,
                  ((struct multiboot_tag_bootdev *) tag)->part);
                break;
            default:
                debug("Found multiboot tag with unused type: 0x%x(%d)", tag->type, tag->type);
                break;
        }
    }
    bootinfo.mem_size = memreg_ptr;
    bootinfo.framebuffer = (uint64_t)fb;
    bootinfo.framebuffer &= 0xFFFFFFFF;
    bootinfo.tss         = &tss_entry;
    info("Placed bootinfo at 0x%x (%d entries)", &bootinfo, bootinfo.mem_size);
    uint32_t entry = load_elf64(kernel_start);
    setup_longmode();
    enter_kernel(entry, &bootinfo);
}