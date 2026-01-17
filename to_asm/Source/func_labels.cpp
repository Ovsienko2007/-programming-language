#include "tree.h"

static const int start_func_arr_capacity = 16;

func_arr_t init_func_arr(){
    func_arr_t new_func_arr = {
        .func = (func_name_t *)calloc(start_func_arr_capacity, sizeof(func_name_t)),
        .size = 0,
        .capacity = start_func_arr_capacity,
    };

    return new_func_arr;
}

void destroy_func_arr(func_arr_t func_arr){
    free(func_arr.func);
}

int find_func_arr(func_arr_t *func_arr, char *func_name, int *num_of_pars){
    assert(func_arr);
    assert(func_name);
    assert(num_of_pars);

    for (size_t pos_func_arr = 0; pos_func_arr < func_arr->size; pos_func_arr++){
        if (strcmp(func_name, func_arr->func[pos_func_arr].func_name) == 0){

            *num_of_pars = func_arr->func[pos_func_arr].nam_of_pars;

            return func_arr->func[pos_func_arr].label;
        }
    }

    return -1;
}

void add_elem_func_arr(func_arr_t *func_arr, char *func_name, int num_of_pars, int *free_label){

    printf("%d", *free_label);
    assert(func_arr);
    assert(func_name);
    assert(free_label);

    func_arr->func[func_arr->size].func_name   = func_name;
    func_arr->func[func_arr->size].nam_of_pars = num_of_pars;
    func_arr->func[func_arr->size].label       = *free_label;

    func_arr->size++;

    (*free_label)++;

    if (func_arr->size == func_arr->capacity){
        func_arr->capacity *= 2;

        func_name_t *new_func_arr = (func_name_t *)calloc(func_arr->capacity, sizeof(func_name_t));
        if (!new_func_arr) return;

        func_arr->func = new_func_arr;
    }
}