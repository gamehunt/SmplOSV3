#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <loader/io.h>
#include <loader/util/printf.h>
#include <multiboot2.h>
#include <loader/log.h>

 
#if defined(__linux__)
#error "Cant find cross compiler"
#endif
 
#if !defined(__i386__)
#error "Invalid arch, need i686-elf"
#endif

void loader_main(uint32_t mbt, uint32_t magic) 
{
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC){
        error("Multiboot check failed!");
        return;
    }

    if (mbt & 7)
    {
        error ("Unaligned multiboot info: 0x%x\n", mbt);
        return;
    }

    struct multiboot_tag* tag;
    struct multiboot_tag_framebuffer *tagfb; 
    multiboot_memory_map_t *tag_mmap;
    uint32_t *fb;
    uint32_t size;
    size = *(uint32_t*)mbt;
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
                    printf("--> 0x%x%x - 0x%x%x - 0x%x\n\r", 
                    (unsigned) (tag_mmap->addr >> 32), 
                    (unsigned) (tag_mmap->addr & 0xffffffff), 
                    (unsigned) (tag_mmap->len >> 32), 
                    (unsigned) (tag_mmap->len & 0xffffffff),
                    (unsigned) (tag_mmap->type));
                }
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                info ("Module at 0x%x-0x%x. Command line %s",
                  ((struct multiboot_tag_module *) tag)->mod_start,
                  ((struct multiboot_tag_module *) tag)->mod_end,
                  ((struct multiboot_tag_module *) tag)->cmdline);
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
}