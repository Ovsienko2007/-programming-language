#include "tree.h"

static void skip_spaces(str_code *data);

static const int start_tokens_arr_capacity = 16;
static const int start_var_name_size       = 16;

static code_tree_t get_var(str_code *data);
static code_tree_t get_number(str_code *data);

tokens_arr_t analysis(const char* file_name){
    assert(file_name);

    str_code     data   =  get_data(file_name);
    if (data.size == 0) return {};

    tokens_arr_t tokens_arr = {
        .size     = 0,
        .capacity = start_tokens_arr_capacity,
        .node_arr  = (node_t **)calloc(start_tokens_arr_capacity, sizeof(node_t *)),
        .pos      = 0
    };

    skip_spaces(&data);
    while (*data.data != '\0'){
        node_t *new_node = nullptr;

        if (*data.data == '['){
            do{
                data.data++;
            }
            while (*(data.data - 1) != ']');    
        }

        for (size_t check_num = 0; check_num < op_list_size; check_num++){
            if (strncmp(op_list[check_num].char_op, data.data, 
                        (size_t)op_list[check_num].size) == 0){
                new_node = create_node(op_list[check_num].op);

                data.data += op_list[check_num].size;
                break;
            }
        }

        if (!new_node) new_node = get_number(&data);

        if (!new_node) new_node = get_var(&data);

        if (!new_node) {
            data.error = true;
            break;
        }

        add_elem_to_tokens_arr(&tokens_arr, new_node);

        skip_spaces(&data);
    }

    destroy_strcode(data);

    return tokens_arr;
}

void add_elem_to_tokens_arr(tokens_arr_t *tokens_arr, node_t *new_token){
    tokens_arr->node_arr[tokens_arr->size] = new_token;
    tokens_arr->size++;

    if (tokens_arr->size == tokens_arr->capacity){
        tokens_arr->capacity *= 2;
        node_t **new_tokens_arr = (node_t **)realloc(tokens_arr->node_arr, 
                                                     tokens_arr->capacity * sizeof(node_t *));
        if (!new_tokens_arr) return;

        tokens_arr->node_arr = new_tokens_arr;
    }
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

    if ('a' <= *data->data && *data->data <= 'z'){
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

void destroy_tokens_arr(tokens_arr_t tokens_arr){
    for (size_t token_pos = 0; token_pos < tokens_arr.size; token_pos++){
        if (tokens_arr.node_arr[token_pos]->type == VAR){
            free(tokens_arr.node_arr[token_pos]->val.var);
        }
        free(tokens_arr.node_arr[token_pos]);
    }

    free(tokens_arr.node_arr);
}