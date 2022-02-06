#include <boot/bootinfo.h>
#include <shared.h>

#include <dev/framebuffer.h>
#include <dev/serial.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

void kernel_main(bootinfo_t* info){
    k_fb_setup(info->framebuffer);
    k_fb_print_string(10, 30 + info->icon->h, "[I] Kernel loaded.");

    printf("AAAA BBB %s 0x%x\r\n", "DDDD", 120312);
}