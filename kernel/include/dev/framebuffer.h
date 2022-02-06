#ifndef _K_FRAMEBUFFER_H
#define _K_FRAMEBUFFER_H

#include <commons.h>
#include <boot/bootinfo.h>

/* Scalable Screen Font (https://gitlab.com/bztsrc/scalable-font2) */
typedef struct {
    unsigned char  magic[4];
    unsigned int   size;
    unsigned char  type;
    unsigned char  features;
    unsigned char  width;
    unsigned char  height;
    unsigned char  baseline;
    unsigned char  underline;
    unsigned short fragments_offs;
    unsigned int   characters_offs;
    unsigned int   ligature_offs;
    unsigned int   kerning_offs;
    unsigned int   cmap_offs;
} __attribute__((packed)) ssfn_font_t;

void k_fb_setup(fb_info_t* info);
void k_fb_print_string(int x, int y, char *s);

#endif