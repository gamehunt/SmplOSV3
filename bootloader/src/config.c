#include <config.h>
#include <shared.h>
#include <log.h>

static char* __fixed_strtok(char* s, char d)
{
    static char* input = NULL;
 
    if (s != NULL)
        input = s;

    if (input == NULL)
        return NULL;
 
    char* result = malloc(strlen(input) + 1);
    int i = 0;

    for (; input[i] != '\0'; i++) {
 
        if (input[i] != d)
            result[i] = input[i];

        else {
            result[i] = '\0';
            input = input + i + 1;
            return result;
        }
    }

    result[i] = '\0';
    input = NULL;

    return result;
}

static BSTATUS __try_parse(char* data, config_result_t* result){

    UNUSED(result);

    char * pch = __fixed_strtok (data, '\n');
 
    result->child_count = 0;

    while (pch != NULL && strlen(pch) > 0)
    {
      result->child_count++;
      if(!result->childs){
          result->childs = malloc(sizeof(config_node_t) * result->child_count);
      }else{
          result->childs = realloc(result->childs, sizeof(config_node_t) * result->child_count);
      }

      char* name = malloc(strlen(pch));
      char* value = malloc(strlen(pch));
      char* del = strchr(pch, '=');

      if(del == NULL){
          free(name);
          free(value);
          free(pch);
          return 1;
      }

      uintptr_t pos = del - pch;

      memcpy(name, pch, pos);
      name[pos] = '\0';

      int len = strlen(pch) - pos;

      memcpy(value, &pch[pos + 1], len);
      value[len] = '\0';

      result->childs[result->child_count - 1].name = malloc(strlen(name));
      strcpy(result->childs[result->child_count - 1].name, name);

      result->childs[result->child_count - 1].value = malloc(strlen(value));
      strcpy(result->childs[result->child_count - 1].value, value);

      free(name);
      free(value);
      free(pch);

      pch = __fixed_strtok (NULL, '\n');
    }

    return 0;
}

BSTATUS b_config_parse(const char* path, config_result_t* result){

    if(!result){
        return CFG_ERR_INVALID_ARGS;
    }

    FILE *f;
    char *buff;
    long int size;

    if((f = fopen(path, "r"))) {

        fseek(f, 0, SEEK_END);
        size = ftell(f);

        fseek(f, 0, SEEK_SET);
        buff = malloc(size + 1);

        if(!buff) {
            result->error.status = CFG_ERR_GENERIC;
            result->error.description = "Unable to allocate memory.";
            return CFG_ERR_GENERIC;
        }

        fread(buff, size, 1, f);
        buff[size] = 0;

        fclose(f);
        
        result->child_count = 0;
        result->childs = 0;

        if(__try_parse(buff, result)){
            result->error.status = CFG_ERR_PARSING_FAILED;
            result->error.description = "Failed to parse config file.";
            return CFG_ERR_PARSING_FAILED;
        }

        return CFG_ERR_SUCCESS;
    } else {
        result->error.status = CFG_ERR_READ_FAILED;
        result->error.description = "Failed to read config file.";

        return CFG_ERR_READ_FAILED;
    }
}