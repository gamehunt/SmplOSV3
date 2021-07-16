#include <dev/log.h>
#include <dev/pit.h>
#include <dev/pci.h>
#include <util.h>
#include <interrupts.h>
#include <loader/bootinfo.h>
#include <memory.h>
#include <fs/tar.h>
#include <elf64.h>
#include <module.h>
#include <stdlib.h>

#define SHORT_ASSERT
#include <assert.h>

void kernel_main(bootinfo_t *bootinfo)
{
    info("Reached kernel entrypoint, bootinfo=0x%llx (%d entries, tss=0x%llx)", bootinfo, bootinfo->mem_size, bootinfo->tss);
    setup_interrupts();
    info("Initializing MMU");
    for (uint8_t i = 0; i < bootinfo->mem_size; i++)
    {
        uint64_t start, end;
        uint8_t type;
        start = (((bootinfo_mem_region_t *)bootinfo->memory)[i]).start;
        end = (((bootinfo_mem_region_t *)bootinfo->memory)[i]).end;
        type = (((bootinfo_mem_region_t *)bootinfo->memory)[i]).type;
        info("0x%llx - 0x%llx - %x", start, end, type);
        if (type == 1)
        {
            for (uint64_t addr = start; addr <= end; addr += 4096)
            {
                if (addr > 0x13ff000)
                {
                    pmm_mark_frame(addr);
                }
            }
        }
    }
    info("PMM stack size = %d KB (end at 0x%llx)", pmm_get_stack_size() * 8 / 1024, PMM_STACK_ADDRESS + pmm_get_stack_size() * 8);
    init_paging();

    init_heap(MB(16));

    info("Mapping framebuffer at 0x%llx", bootinfo->framebuffer);

    for (uint32_t i = 0; i < 1024 * 768 * 32; i += 4096)
    {
        mapf(bootinfo->framebuffer + i, bootinfo->framebuffer + i, PAGE_FLAG_WRITABLE | PAGE_FLAG_USER);
    }

    tss_entry_t *tss = (tss_entry_t *)bootinfo->tss;
    info("Kernel stack at 0x%llx", tss->rsp0);

    pci_init();
    //while(1);

    export_symbols();
    pit_phase(1000);

    if (CHECK_ADDR(bootinfo->initrd))
    {
        info("Parsing ramdisk");

        uint64_t init = 0;

        tar_parsed_t *parsed = tar_parse(bootinfo->initrd);
        for (int i = 0; i < parsed->entry_count; i++)
        {
            tar_file_t *file = parsed->entries[i];
            if (endswith(file->filename, ".smpm"))
            {
                info("Loading module - %s 0x%llx %d B", file->filename, file->start, file->size);
                load_module(file->start);
            }
            else if (endswith(file->filename, ".smpinit"))
            {
                init = file->start;
            }
        }

        if (!check_required_modules())
        {
            error("Not all required modules have been loaded, halting kernel.");
            error("Faulty/Non-existent modules:");
            const char **required = get_required_modules_left();
            for (int i = 0; i < REQUIRED_MODULES_TOTAL; i++)
            {
                if (strlen(required[i]))
                {
                    error("* %s", required[i]);
                }
            }
        }
        else
        {
            if (init)
            {
                exec(init, "init", 1);
            }
            else
            {
                error("Can't find initial executable, halting kernel.");
            }
        }
    }
    else
    {
        error("Ramdisk not found, halting kernel.");
    }

    for (;;)
    {
        asm("hlt");
    }
}

void _start(bootinfo_t *bootinfo)
{
    kernel_main(bootinfo);
}