#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <io.h>
#include <multiboot2.h>
 
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a i686-elf compiler"
#endif
 


size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

int is_transmit_empty() {
   return inb(0x3F8 + 5) & 0x20;
}

void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outb(0x3F8,a);
}

void terminal_putchar(char c) 
{
	write_serial(c);
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void terminal_writehex(uint32_t data) 
{
	while(data > 0){
        if(data % 16 < 10){
            terminal_putchar('0' + data % 16);
        }else{
            terminal_putchar('A' + data % 16 - 10);
        }
        data/=16;
    }
}
 
void kernel_main(uint32_t mbt, uint32_t magic) 
{
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC){
        terminal_writestring("MULTIBOOT FAILED\n\r");
        return;
    }
    struct multiboot_tag* tag;
    struct multiboot_tag_framebuffer *tagfb; 
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
                terminal_writestring(((struct multiboot_tag_string *) tag)->string);
                terminal_writestring("\n\r");
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                tagfb = (struct multiboot_tag_framebuffer *) tag;
                fb = (uint32_t *) (unsigned long) tagfb->common.framebuffer_addr;
                for(int i=0;i<767;i++){
                    fb[1024*i + i] = 0xFF00FF00;
                }
                for(int i=0;i<767;i++){
                    fb[1024*(767 - i) + i] = 0xFFFFFF00;
                }
            default:
                terminal_writestring("0x");
                terminal_writehex(tag->type);
                terminal_writestring("\n\r");
        }
    }
}