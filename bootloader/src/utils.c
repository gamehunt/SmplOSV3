#include <utils.h>

BSTATUS b_read_file(const char* path, char** buff, uint32_t* size){
    FILE *f;

    if((f = fopen(path, "r"))) {

        fseek(f, 0, SEEK_END);
        *size = ftell(f);

        fseek(f, 0, SEEK_SET);
        *buff = malloc(*size + 1);

        if(!*buff) {
            return 1;
        }

        fread(*buff, *size, 1, f);
        (*buff)[*size] = 0;

        fclose(f);
        
        return 0;
    }

    return 2;
}