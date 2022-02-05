#include <config.h>
#include <log.h>
#include <shared.h>
#include <stddef.h>
#include <elf.h>
#include <bootinfo.h>
#include <mem.h>

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
    efi_gop_mode_info_t *info = NULL;
    UNUSED(info);

    status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);

    if(!EFI_ERROR(status) && gop) {
        bi.framebuffer->addr = gop->Mode->FrameBufferBase;
        bi.framebuffer->size = gop->Mode->FrameBufferSize;
        b_info("Found framebuffer: 0x%x - 0x%x", bi.framebuffer->addr, bi.framebuffer->addr + bi.framebuffer->size);
    }else{
        b_error("Unable to get graphics output protocol.");
        return 0;
    }

    b_info("Trying to parse boot.cfg...");

    config_result_t  data;

    char kernel_path[256];
    memset(kernel_path, 0, 256);

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
            ST->ConOut->Reset(ST->ConOut, 0);
            ST->StdErr->Reset(ST->StdErr, 0);
            if(EFI_ERROR(status)) {
                b_error("Unable to set video mode\n");
            }else{
                b_info("Graphics mode set to %s", data.childs[i].value);
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

    FILE *f;
    char *buff;
    long int size;

    if((f = fopen(kernel_path, "r"))) {
        fseek(f, 0, SEEK_END);
        size = ftell(f);

        fseek(f, 0, SEEK_SET);
        buff = malloc(size + 1);

        if(!buff) {
            b_error("Malloc failure");
            return 0;
        }

        fread(buff, size, 1, f);
        fclose(f);
    }else{
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

    if(exit_bs()){
        b_error("Failed to exit boot services!");
    }else{
        ((void (*)(bootinfo_t*))(entry))(&bi);
    }

    return 0;
}
