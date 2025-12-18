#include "tree.h"

static size_t start_capacity_var_arr = 16;

var_stack_t *init_var_stack(){
    var_stack_t *var_stack = (var_stack_t *)calloc(1, sizeof(var_stack_t));

    var_stack->capacity = start_capacity_var_arr;
    var_stack->var_arr_arr = (var_place_arr_t **)calloc(var_stack->capacity,
                                                 sizeof(var_place_arr_t *));

    return var_stack;
}

void add_elem_to_var_stack(var_stack_t *var_stack, char *new_elem_name){
    add_elem_to_var_arr(var_stack->var_arr_arr[var_stack->size - 1], new_elem_name, 
                       &(var_stack->free));
}

void push_var_stack(var_stack_t *var_arr){
    var_arr->var_arr_arr[var_arr->size] = init_var_arr();
    var_arr->size++;

    if (var_arr->size == var_arr->capacity){
        var_arr->capacity *= 2;
        var_arr->var_arr_arr = (var_place_arr_t **)realloc(var_arr->var_arr_arr, 
                                                  var_arr->capacity * sizeof(var_place_arr_t *));
    }
}

int find_elem_var_stack(var_stack_t *var_stack, char *var_name){
    int pos = -1;

    for (size_t pos_var_stack = 0; pos_var_stack < var_stack->size; pos_var_stack++){
        pos = get_name_from_arr(var_stack->var_arr_arr[pos_var_stack], var_name);

        if (pos != -1) break;
    }

    return pos;
}

void pop_var_stack(var_stack_t *var_stack){
    destroy_var_arr(var_stack->var_arr_arr[var_stack->size - 1]);

    var_stack->size--;
}

void destroy_var_stack(var_stack_t *var_stack){
    while (var_stack->size) pop_var_stack(var_stack);

    free(var_stack);
}

var_place_arr_t *init_var_arr(){
    var_place_arr_t *new_var_arr = (var_place_arr_t *)calloc(1, sizeof(var_place_arr_t));

    new_var_arr->capacity = start_capacity_var_arr;
    new_var_arr->var_arr = (var_place_t *)calloc(new_var_arr->capacity,
                                                 sizeof(var_place_t));

    return new_var_arr;
}

void add_elem_to_var_arr(var_place_arr_t *var_arr, char *new_elem_name, int *pos){
    var_arr->var_arr[var_arr->size] = {new_elem_name, *pos};
    fprintf(stderr, "efvuhdjrftbhij\n\n");
    fprintf(stderr, "%s\n", new_elem_name);
    *pos += 1;
    var_arr->size++;

    if (var_arr->size == var_arr->capacity){
        var_arr->capacity *= 2;
        var_arr->var_arr = (var_place_t *)realloc(var_arr->var_arr, 
                                                  var_arr->capacity * sizeof(var_place_t));
    }
}

int get_name_from_arr(var_place_arr_t *var_arr, char *new_elem_name){
    for (size_t var_pos = 0; var_pos < var_arr->size; var_pos++){
        if (strcmp(new_elem_name, var_arr->var_arr[var_pos].name) == 0){
            return var_arr->var_arr[var_pos].place;
        }
    }

    return -1;
}

void destroy_var_arr(var_place_arr_t *varr_arr){
    free(varr_arr->var_arr);
    free(varr_arr);
}

void print_dump(var_stack_t *var_stack){
    fprintf(stderr, "=================DUMP=================\n");

    fprintf(stderr, "STACK[%p]\n", var_stack);

    fprintf(stderr, "size     %zu\n", var_stack->size);
    fprintf(stderr, "capacity %zu\n\n", var_stack->capacity);

    fprintf(stderr, "free %d\n\n", var_stack->free);

    fprintf(stderr, "DATA:\n");

    for (size_t i = 0; i < var_stack->size; i++){
        fprintf(stderr, "_________%zu level_____________\n", i);
        fprintf(stderr, "size     %zu\n", var_stack->var_arr_arr[i]->size);
        fprintf(stderr, "capacity %zu\n\n", var_stack->var_arr_arr[i]->size);

        for (size_t j = 0; j < var_stack->var_arr_arr[i]->size; j++){
            fprintf(stderr, "%s\t[%d]\n", var_stack->var_arr_arr[i]->var_arr[j].name, 
                                          var_stack->var_arr_arr[i]->var_arr[j].place);
            fprintf(stderr, "!!!");
        }
    }

    fprintf(stderr, "===================end================\n");
}