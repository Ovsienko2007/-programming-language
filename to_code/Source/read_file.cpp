#include "tree.h"
#include <fcntl.h>
#include <unistd.h>

static off_t find_file_size(const char * file_name);

void get_file_name(int argc, char **argv, const char **source_name, const char **dest_name){
    assert(argv);
    assert(source_name);
    assert(dest_name);

    bool source_name_is_get = false, dest_name_is_get = false;

    for (int arg_pos = 1; arg_pos < argc - 1; arg_pos++){
        if (strcmp(argv[arg_pos], "-s") == 0){
            source_name_is_get = true;
            *source_name = argv[arg_pos + 1];
            arg_pos++;
        }
        if (strcmp(argv[arg_pos], "-d") == 0){
            dest_name_is_get = true;
            *dest_name = argv[arg_pos + 1];
            arg_pos++;
        }
    }
    if (!source_name_is_get){
        *source_name = "result_tree.txt";
    }
    if (!dest_name_is_get){
        *dest_name = "result_code.asm";
    }
}

str_code get_data(const char *file_name){
    assert(file_name);
    str_code data = {};

    data.size = find_file_size(file_name) + 1;

    data.start_pos = data.data = (char *)calloc((size_t)data.size , sizeof(char));

    int file_descriptor = open(file_name, O_RDONLY);
    
    if (file_descriptor == -1){
        close(file_descriptor);
        return data;
    }

    data.size = read(file_descriptor, data.data, (size_t)data.size);

    close(file_descriptor);

    if (data.size == -1) return {};

    data.data[data.size] = '\0';

    return data;
}

static off_t find_file_size(const char * file_name){
    assert(file_name);

    struct stat file_stat;

    if (stat(file_name, &file_stat) == -1) return 0;

    return file_stat.st_size;
}

void destroy_strcode(str_code data){
    if (data.start_pos) free(data.start_pos);
}

