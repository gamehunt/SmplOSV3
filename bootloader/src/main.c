#include <config.h>
#include <log.h>
#include <shared.h>
#include <stddef.h>
#include <elf.h>
#include <bootinfo.h>
#include <mem.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    ST->ConOut->ClearScreen(ST->ConOut);
    ST->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

    b_info("SmplOSV3.1 Loader started");
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

    b_setup_paging();

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

    bootinfo_t bi;
    if(exit_bs()){
        b_error("Failed to exit boot services!");
    }else{
        ((void (*)(bootinfo_t*))(entry))(&bi);
    }

    return 0;
}
