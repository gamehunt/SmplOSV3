#include <config.h>
#include <log.h>
#include <shared.h>
#include <stddef.h>


int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    ST->ConOut->ClearScreen(ST->ConOut);
    ST->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    
    b_info("SmplOSV3.1 Loader started");
    b_info("Trying to parse boot.cfg...");

    config_result_t  data;
    if(b_config_parse("\\smplos\\boot.cfg", &data)){
        b_warn("Failed to parse config: %s", data.error.description);
    }else{
        for(uint32_t i=0;i<data.child_count;i++){
          if(!strcmp(data.childs[i].name, "kernel")){
              b_info("Found kernel entry: %s", data.childs[i].value);
          } 
          else if(!strcmp(data.childs[i].name, "video")){
              b_info("Found graphics mode entry: %s", data.childs[i].value);
          }   
        }
    }

    getchar();
    return 0;
}
