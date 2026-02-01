#include "tree.h"

static code_tree_t get_tree_from_str_code(str_code *data);
static void        skip_spaces(str_code *data);

static code_tree_t get_var(str_code *data);
static code_tree_t get_number(str_code *data);

static const int   start_var_name_size = 16;

static const char null_ptr_name[]       = "nil";

code_tree_t get_tree_from_file(const char * source_name){
    assert(source_name);

    str_code data = get_data(source_name);

    code_tree_t new_tree = get_tree_from_str_code(&data);

    if (data.error){
        destroy_tree(new_tree);

        printf("ERROR\n");

        new_tree = nullptr;    
    }
    
    destroy_strcode(data);

    return new_tree;
}

static code_tree_t get_tree_from_str_code(str_code *data){
    assert(data);

    if (data->error) return nullptr;

    skip_spaces(data);

    if (strncmp(data->data, null_ptr_name, sizeof(null_ptr_name) - 1) == 0){
        data->data += sizeof(null_ptr_name) - 1;
        return nullptr;
    }

    if (*data->data != '(') {
        data->error = true;
        return nullptr;
    }
    data->data++;

    skip_spaces(data);

    node_t *new_node = nullptr;

    for (size_t check_num = 0; check_num < op_list_size; check_num++){
        if (data->data[op_list[check_num].size] == ' ' &&
            strncmp(op_list[check_num].for_tree, data->data, 
                    (size_t)op_list[check_num].size) == 0){
            new_node = create_node(op_list[check_num].op);

            data->data += op_list[check_num].size;
            break;
        }
    }

    if (!new_node) new_node = get_number(data);

    if (!new_node) new_node = get_var(data);

    if (!new_node) {
        data->error = true;
        return nullptr;
    }

    new_node->left_node = get_tree_from_str_code(data);

    new_node->right_node = get_tree_from_str_code(data);

    skip_spaces(data);

    if (*data->data != ')') {
        fprintf(stderr, "!!!");
        data->error = true;

        free(new_node);

        return nullptr;
    }
    data->data++;

    return new_node;
}

static void skip_spaces(str_code *data){
    assert(data);

    for (; *data->data == ' '  || *data->data == '\n' ||
           *data->data == '\t' || *data->data == '\r'   ; data->data++);
}

static code_tree_t get_number(str_code *data ){
    assert(data);

    node_t *par = nullptr;

    if ('0' <= *data->data && *data->data <= '9'){
        int x         = 0;
        bool   is_n      = false;

        while ('0' <= *data->data && *data->data <= '9'){
            is_n = true;

            x = x * 10 + (*data->data - '0');

            data->data++;
        }

        if (!is_n){
            data->error = true;
        }

        par = create_num_node(x);
    }

    return par;
}

static code_tree_t get_var(str_code *data ){
    assert(data);

    node_t *new_node = nullptr;

    if (('a' <= *data->data && *data->data <= 'z') ||
        ('A' <= *data->data && *data->data <= 'Z') ){
        var_name_t var_name = {
            .size     = 0,
            .capacity = start_var_name_size,
            .name     = (char *)calloc(start_var_name_size, sizeof(char))
        };

        do{
            var_name.name[var_name.size] = *data->data;
            var_name.size++;
            data->data++;

            if (var_name.size == var_name.capacity){
                var_name.capacity *= 2;
                char *new_name = (char *)realloc(var_name.name, var_name.capacity * sizeof(char));

                if (!new_name) {
                    return nullptr;
                }

                var_name.name = new_name;
            }

        } while (('a' <= *data->data && *data->data <= 'z') || 
                 ('A' <= *data->data && *data->data <= 'Z') ||
                 ('0' <= *data->data && *data->data <= '9') ||
                  '_' == *data->data);

        return create_var_node(var_name.name);
    }

    return new_node;
}