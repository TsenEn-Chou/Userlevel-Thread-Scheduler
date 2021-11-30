#include "os2021_thread_api.h"
#include "../json-c/json.h"

void JsonParse(char *file_name);

int main(int argc ,char** argv){
    
    InitAllQueues();
    JsonParse("init_threads.json");
    StartSchedulingSimulation();
    return 0;
}

void JsonParse(char *file_name){
    FILE *fp;
    int i,j;
    char buffer[1024];
    json_object *parsed_json;
    json_object *threads;
    json_object *thread;

    json_object *name;
    json_object *entry_function;
    json_object *priority;
    json_object *cancel_mode;

    size_t n_threads;

    fp = fopen(file_name,"r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "Threads", &threads);
    n_threads = json_object_array_length(threads);
    printf("Found %lu threads\n",n_threads);

    for(i=0;i<n_threads;i++){
        thread = json_object_array_get_idx(threads, i);
        json_object_object_get_ex(thread,"name", &name);
        json_object_object_get_ex(thread,"entry function", &entry_function);
        json_object_object_get_ex(thread,"priority", &priority);
        json_object_object_get_ex(thread,"cancel mode", &cancel_mode);

        switch (*json_object_get_string(priority))
        {
            case 'H':
                j = 0;
                break;
            case 'M':
                j = 1;
                break;
            case 'L':
                j = 2;
                break;
            default:
                break;
        }

        OS2021_ThreadCreate((char*)json_object_get_string(name), 
        (char*)json_object_get_string(entry_function), 
        j, 
        json_object_get_int(cancel_mode)); 
    }
}