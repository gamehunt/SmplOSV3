#include <config.h>
#include <log.h>
#include <shared.h>
#include <stddef.h>
#include <elf.h>
#include <bootinfo.h>
#include <utils.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static uint8_t get_memory_type(uint8_t type){
    if(type == 1 || type == 2 || type == 3 || type == 4 || type == 7){
        return B_MEMTYPE_ALLOWED;
    }else if(type == 9){
        return B_MEMTYPE_ACPI;
    }else{
        return B_MEMTYPE_RESERVED;
    }
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    ST->ConOut->ClearScreen(ST->ConOut);
    ST->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    bootinfo_t bi;
    bi.icon = NULL;

    b_info("-------------SmplOSV3.1 Loader-------------");
    b_info("Retrieving memory map...");

    efi_status_t status;
    efi_memory_descriptor_t *memory_map = NULL, *mement;
    uintn_t memory_map_size=0, map_key=0, desc_size=0;

    status = BS->GetMemoryMap(&memory_map_size, NULL, &map_key, &desc_size, NULL);
    if(status != EFI_BUFFER_TOO_SMALL || !memory_map_size){
        b_error("Failed to get memory map.");
        return 0;
    }

    memory_map_size += 4 * desc_size;
    memory_map = (efi_memory_descriptor_t*)malloc(memory_map_size);
    if(!memory_map) {
        b_error("Malloc failure");
        return 0;
    }

    status = BS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &desc_size, NULL);
    if(EFI_ERROR(status)) {
        b_error("Failed to get memory map.");
        return 0;
    }

    bi.memmap = malloc(sizeof(memmap_info_t));
    bi.memmap->size = 0;
    bi.memmap->entries = NULL;

    for(mement = memory_map; (uint8_t*)mement < (uint8_t*)memory_map + memory_map_size;
        mement = NextMemoryDescriptor(mement, desc_size)) {
        if(bi.memmap->entries && bi.memmap->entries[bi.memmap->size - 1].type == get_memory_type(mement->Type) && ((bi.memmap->entries[bi.memmap->size - 1].start + bi.memmap->entries[bi.memmap->size - 1].size * 0x1000) == mement->PhysicalStart)){
            bi.memmap->entries[bi.memmap->size - 1].size += mement->NumberOfPages;
            continue;
        }
        
        bi.memmap->size++;
        if(!bi.memmap->entries){
            bi.memmap->entries = malloc(sizeof(memmap_entry_t));
        }else{
            bi.memmap->entries = realloc(bi.memmap->entries, bi.memmap->size*sizeof(memmap_entry_t));
        }
        bi.memmap->entries[bi.memmap->size - 1].start = mement->PhysicalStart;
        bi.memmap->entries[bi.memmap->size - 1].size = mement->NumberOfPages;
        bi.memmap->entries[bi.memmap->size - 1].type = get_memory_type(mement->Type);
    }

    free(memory_map);

    for (uint32_t i = 0; i < bi.memmap->size-1; i++){
       for (uint32_t j = 0; j < bi.memmap->size-i-1; j++){
           if (bi.memmap->entries[j].start > bi.memmap->entries[j+1].start){
              memmap_entry_t t = bi.memmap->entries[j];
              bi.memmap->entries[j] = bi.memmap->entries[j + 1];
              bi.memmap->entries[j + 1] = t;
           }
       }
    }

    for (uint32_t i = 0; i < bi.memmap->size; i++){
        if(bi.memmap->entries[i].type == B_MEMTYPE_ALLOWED){
            b_info("Usable memory range found: 0x%x - 0x%x", bi.memmap->entries[i].start, bi.memmap->entries[i].start + bi.memmap->entries[i].size * 0x1000);
        }
    }

    b_info("Getting framebuffer info...");

    bi.framebuffer = malloc(sizeof(fb_info_t));

    efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    efi_gop_t *gop = NULL;

    status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);

    char* names[] = {
        "8bit RGB",
        "8bit BGR",
        "bitmash",
        "blt only",
        "max"
    };

    if(!EFI_ERROR(status) && gop) {
        
        efi_status_t status = gop->SetMode(gop, gop->Mode->Mode);
        if (EFI_ERROR(status)) {
            b_error("Failed to set default graphics mode.");
            return 0;
        }

        bi.framebuffer->addr                = gop->Mode->FrameBufferBase;
        bi.framebuffer->size                = gop->Mode->FrameBufferSize;
        bi.framebuffer->width               = gop->Mode->Information->HorizontalResolution;
        bi.framebuffer->height              = gop->Mode->Information->VerticalResolution;
        bi.framebuffer->pixels_per_scanline = gop->Mode->Information->PixelsPerScanLine;
        bi.framebuffer->pixel_format        = gop->Mode->Information->PixelFormat;
        
        b_info("Found framebuffer: [%d] 0x%x - 0x%x (%dx%d, %dpps %s)",
             gop->Mode->Mode,
             bi.framebuffer->addr, 
             bi.framebuffer->addr + bi.framebuffer->size, 
             bi.framebuffer->width, bi.framebuffer->height, 
             bi.framebuffer->pixels_per_scanline, 
             names[bi.framebuffer->pixel_format]);
    }else{
        b_error("Unable to get graphics output protocol.");
        return 0;
    }

    b_info("Trying to parse boot.cfg...");

    config_result_t  data;

    char kernel_path[256];
    memset(kernel_path, 0, 256);

    uint32_t size;

    if(b_config_parse("\\smplos\\boot.cfg", &data)){
        char* msg;
        switch(data.status){
            case CFG_ERR_INVALID_ARGS:
                msg = "Invalid arguments";
                break;
            case CFG_ERR_PARSING_FAILED:
                msg = "Syntax error";
                break;
            case CFG_ERR_READ_FAILED:
                msg = "Failed to read file";
                break;
            case CFG_ERR_GENERIC:
            default:
                msg = "Unknown error";
                break;
        }
        b_warn("Failed to parse config: %s", msg);
    }else{
        for(uint32_t i=0;i<data.child_count;i++){
          if(!strcmp(data.childs[i].name, "kernel")){
              b_info("Found kernel entry: %s", data.childs[i].value);
              sprintf(&kernel_path[0], "\\smplos\\%s", data.childs[i].value);
          } 
          else if(!strcmp(data.childs[i].name, "video")){
            b_info("Found graphics mode entry: %s", data.childs[i].value);

            status = gop->SetMode(gop, atoi(data.childs[i].value));            

            if(EFI_ERROR(status)) {
                b_error("Unable to set video mode\n");
            }else{
                ST->ConOut->ClearScreen(ST->ConOut);

                bi.framebuffer->addr                = gop->Mode->FrameBufferBase;
                bi.framebuffer->size                = gop->Mode->FrameBufferSize;
                bi.framebuffer->width               = gop->Mode->Information->HorizontalResolution;
                bi.framebuffer->height              = gop->Mode->Information->VerticalResolution;
                bi.framebuffer->pixels_per_scanline = gop->Mode->Information->PixelsPerScanLine;
                bi.framebuffer->pixel_format        = gop->Mode->Information->PixelFormat;

                b_info("Graphics mode set to %d (%dx%d, %dpps %s)", gop->Mode->Mode,  bi.framebuffer->width, bi.framebuffer->height, bi.framebuffer->pixels_per_scanline, names[bi.framebuffer->pixel_format]);
            }
          }else if(!strcmp(data.childs[i].name, "icon")){
            b_info("Found icon entry: %s", data.childs[i].value);

            char* icon = NULL;
            uint32_t* image_data;
            char  icon_path[256];
            memset(&icon_path[0], 0, 256);

            int w,h,l;

            sprintf(&icon_path[0], "\\smplos\\%s", data.childs[i].value);

            if(b_read_file(icon_path, &icon, &size)){
                b_warn("Failed to read icon file, skipping...");
            }else{
                stbi__context s;
                stbi__result_info ri;

                ri.bits_per_channel = 8;
                s.read_from_callbacks = 0;
                s.img_buffer = s.img_buffer_original = (uint8_t*)icon;
                s.img_buffer_end = s.img_buffer_original_end = (uint8_t*)icon + size;

                image_data = (uint32_t*)stbi__png_load(&s, &w, &h, &l, 4, &ri);

                if(gop->Mode->Information->PixelFormat == PixelBlueGreenRedReserved8BitPerColor ||
                    (gop->Mode->Information->PixelFormat == PixelBitMask && gop->Mode->Information->PixelInformation.BlueMask != 0xff0000)) {
                    for(l = 0; l < w * h; l++)
                        image_data[l] = ((image_data[l] & 0xff) << 16) | (image_data[l] & 0xff00) | ((image_data[l] >> 16) & 0xff);
                }

                bi.icon = malloc(sizeof(icon_info_t));
                bi.icon->data = image_data;
                bi.icon->w = w;
                bi.icon->h = h;
                bi.icon->l = l;

                gop->Blt(gop, image_data, EfiBltBufferToVideo, 0, 0, gop->Mode->Information->HorizontalResolution - w - 20, (20), w, h, 0);
            }
          }      
          free(data.childs[i].name);
          free(data.childs[i].value);
        }
        free(data.childs);
    }

    if(!strlen(kernel_path)){
        b_warn("Failed to find kernel entry, falling back to default");
        strcpy(kernel_path, "\\smplos\\smplos.elf");
    }

    char *buff = NULL;

    if(b_read_file(kernel_path, &buff, &size)) {
        b_error("Failed to load kernel: Failed to read file");
        return 0;
    }

    uintptr_t entry;
    BSTATUS s;
    if((s = load_elf(buff, &entry))){
        char* msg;
        switch(s){
            case ELF_ERR_INVALID_FORMAT:
                msg = "Invalid format";
                break;
            case ELF_ERR_GENERIC:
            default:
                msg = "Unknown error";
                break;
        }
        b_error("Failed to load kernel: %s", msg);
        return 0;
    }else{
        b_info("Kernel entry point: 0x%x", entry);
    }

    ST->ConOut->ClearScreen(ST->ConOut);
    if(bi.icon){
        uint32_t buffer = 0xFFFFFF;
        gop->Blt(gop, &buffer   , EfiBltVideoFill    , 0, 0, 0, 25 + bi.icon->h, gop->Mode->Information->HorizontalResolution, 1, 0);
    }

    if(exit_bs()){
        b_error("Failed to exit boot services!");
    }else{
        ((void (*)(bootinfo_t*))(entry))(&bi);
    }

    return 0;
}
