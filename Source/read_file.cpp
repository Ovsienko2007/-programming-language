#include "tree.h"

static const int start_var_name_size = 16;

struct str_code{
    int   pos;
    int   size;
    char *data;
    bool  error;
};

static str_code get_data(const char *file_name);

static str_code get_data(const char *file_name){
    str_code data = {};

    FILE *stream = fopen(file_name, "r");
    if (!stream) return data;

    fscanf(stream, "%m[^\n]%n", &data.data, &data.size);

    return data;
}

static expression_t get_number               (str_code *data, var_arr_t *var_arr);
static expression_t get_primary_expression   (str_code *data, var_arr_t *var_arr);
static expression_t get_expression           (str_code *data, var_arr_t *var_arr);
static expression_t get_term                 (str_code *data, var_arr_t *var_arr);
static expression_t get_var                  (str_code *data, var_arr_t *var_arr);
static expression_t get_one_param_func       (str_code *data, var_arr_t *var_arr);
static expression_t get_comparison_operators (str_code *data, var_arr_t *var_arr);
static expression_t get_and_operator         (str_code *data, var_arr_t *var_arr);
static expression_t get_or_operator          (str_code *data, var_arr_t *var_arr);

static void         skip_spaces              (str_code *data);

expression_t get_general(const char* file_name){
    str_code     data   =  get_data(file_name);

    var_arr_t   *var_arr = (var_arr_t *)calloc(1, sizeof(var_arr_t));
    *var_arr = init_var_arr();

    expression_t new_tree = get_or_operator(&data, var_arr);

    skip_spaces(&data);

    if (*data.data != '$'){
        fprintf(stderr, "error");
        data.error = true;
    }
    data.data++;

    if (data.error){
        return nullptr;
    }

    return new_tree;
}

static expression_t get_or_operator (str_code *data, var_arr_t *var_arr){
    node_t *par1 = get_and_operator(data, var_arr);

    skip_spaces(data);
    while (strncmp(data->data, "||", strlen("||")) == 0){
        data->data += strlen("||");

        node_t *par2 = get_and_operator(data, var_arr);

        par1 = create_op_node(OR, par1, par2, var_arr);
    }

    return par1; 
}

static expression_t get_and_operator (str_code *data, var_arr_t *var_arr){
    node_t *par1 = get_comparison_operators(data, var_arr);

    skip_spaces(data);
    while (strncmp(data->data, "&&", strlen("&&")) == 0){
        data->data += strlen("&&");

        node_t *par2 = get_comparison_operators(data, var_arr);

        par1 = create_op_node(AND, par1, par2, var_arr);
    }

    return par1; 
}

static expression_t get_comparison_operators (str_code *data, var_arr_t *var_arr){
    node_t *par1 = get_expression(data, var_arr);

    skip_spaces(data);

    for (bool is_comparison = true; is_comparison; ){
        is_comparison = false;

        for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
            if (op_list[pos_op_list].type == comparison) {
                if (strncmp(data->data, op_list[pos_op_list].char_op, 
                            strlen(op_list[pos_op_list].char_op)) == 0){
                    is_comparison = true;

                    data->data += strlen(op_list[pos_op_list].char_op);

                    node_t *par2 = get_expression(data, var_arr);

                    par1 = create_op_node(op_list[pos_op_list].op, par1, par2, var_arr);
                }
            }
        }
    }

    return par1;
}

static expression_t get_expression(str_code *data, var_arr_t *var_arr){
    node_t *par1 = get_term(data, var_arr);

    skip_spaces(data);
    while (*data->data == '+' || *data->data == '-' || *data->data == '^'){
        char is_sum = *data->data++;

        node_t *par2 = get_term(data, var_arr);

        if (is_sum == '+'){
            par1 = create_op_node(ADD, par1, par2, var_arr);
        }
        else if (is_sum == '-'){
            par1 = create_op_node(SUB, par1, par2, var_arr);
        }
        else{
            par1 = create_op_node(POW, par1, par2, var_arr);
        }
    }

    return par1;
}

static expression_t get_term(str_code *data, var_arr_t *var_arr){
    node_t *par1 = get_primary_expression(data, var_arr);

    skip_spaces(data);
    while (*data->data == '*' || *data->data == '/'){
        bool is_div = (*data->data == '*');

        data->data++;

        node_t *par2 = get_primary_expression(data, var_arr);

        if (is_div){
            par1 = create_op_node(MUL, par1, par2, var_arr);
        }
        else{
            par1 = create_op_node(DIV, par1, par2, var_arr);
        }
    }

    return par1;
}

static expression_t get_primary_expression(str_code *data, var_arr_t *var_arr){
    node_t *ans = nullptr;

    skip_spaces(data);
    if (*data->data == '('){
        data->data++;

        ans = get_expression(data, var_arr);

        skip_spaces(data);
        if (*data->data != ')'){
            data->error = true;
            data->data++;
        }

        data->data++;

        return ans;
    }
    else {
        return get_one_param_func(data, var_arr);
    }
}

static expression_t get_number(str_code *data, var_arr_t *var_arr){
    node_t *par = nullptr;

    skip_spaces(data);
    if ('a' <= *data->data && *data->data <= 'z'){
        par = get_var(data, var_arr);
    }
    if ('0' <= *data->data && *data->data <= '9'){
        double x         = 0;
        bool   is_n      = false;
        double after_dot = 1;

        while (('0' <= *data->data && *data->data <= '9') || 
                *data->data == '.' || *data->data == ','){
            is_n = true;

            if ((*data->data == '.' || *data->data == ',') && 
                 !doubles_are_equal(after_dot, 1)){
                
                data->error = true;
                break;
            }

            if (*data->data == '.' || *data->data == ','){
                after_dot /= 10;
            } 
            else if (!doubles_are_equal(after_dot, 1)){
                x += (*data->data - '0') * after_dot;
                after_dot /= 10;
            }
            else {
                x = x * 10 + (*data->data - '0');
            }

            data->data++;
        }

        if (!is_n){
            data->error = true;
        }

        par = create_num_node(x, var_arr);
    } 

    if (!par) data->error = true;

    return par;
}

static expression_t get_var(str_code *data, var_arr_t *var_arr){
    node_t *new_node = nullptr;

    if ('a' <= *data->data && *data->data <= 'z'){
        var_name_t *var_name = (var_name_t *)calloc(1, sizeof(var_name_t));
        var_name->size     = 0;
        var_name->capacity = start_var_name_size;
        var_name->name     = (char *)calloc(start_var_name_size, sizeof(char));

        do{
            var_name->name[var_name->size] = *data->data;
            var_name->size++;
            data->data++;

            if (var_name->size == var_name->capacity){
                var_name->capacity *= 2;
                char *new_name = (char *)realloc(var_name->name, var_name->capacity * sizeof(char));

                if (!new_name) return nullptr;

                var_name->name = new_name;
            }

        } while (('a' <= *data->data && *data->data <= 'z') || 
                 ('A' <= *data->data && *data->data <= 'Z') ||
                 ('0' <= *data->data && *data->data <= '9') ||
                  '_' == *data->data);

        return create_var_node(var_name->name, var_arr);
    }

    new_node = get_number(data, var_arr);

    if (!new_node){
        data->error = true;
    }

    return new_node;
}

static expression_t get_one_param_func(str_code *data, var_arr_t *var_arr){
    node_t *new_node = nullptr;

    for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
        if (op_list[pos_op_list].num_of_par == 1) {
            if (strncmp(data->data, op_list[pos_op_list].char_op, 
                             strlen(op_list[pos_op_list].char_op)) == 0){
                data->data += strlen(op_list[pos_op_list].char_op);

                return create_op_node(op_list[pos_op_list].op, get_expression(data, var_arr), nullptr, var_arr);
            }
        }
    }

    if (strncmp(data->data, "log_{", strlen("log_{")) == 0){
        data->data += strlen("log_{");

        node_t *par1 = get_expression(data, var_arr);

        if (*data->data != '}'){
            data->error = true;
        }
        data->data++;

        node_t *par2 = get_primary_expression(data, var_arr);

        return create_op_node(LOG, par1, par2, var_arr);
    }

    new_node = get_var(data, var_arr);

    if (!new_node){
        data->error = true;
    }

    return new_node;
}

static void skip_spaces(str_code *data){
    for (; *data->data == ' '; data->data++);
}