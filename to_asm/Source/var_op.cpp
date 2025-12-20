#include "tree.h"

static const int start_capacity = 16;

var_stack_t *init_var_stack(){
    var_stack_t *var_stack = (var_stack_t *)calloc(1, sizeof(var_stack_t));

    var_stack->stack_level.capacity    = start_capacity;
    var_stack->stack_level.stack_level = (size_t *)calloc(start_capacity, sizeof(size_t)); 

    var_stack->var_arr.capacity        = start_capacity;
    var_stack->var_arr.var_arr         = (char **)calloc(start_capacity, sizeof(char *));

    return var_stack;
}

void add_elem_to_var_stack(var_stack_t *var_stack, char *new_elem_name){
    var_stack->var_arr.var_arr[var_stack->var_arr.size] = new_elem_name;
    var_stack->var_arr.size++;

    if (var_stack->var_arr.size > var_stack->max_free_pos){
        var_stack->max_free_pos = var_stack->var_arr.size;
    }

    if (var_stack->var_arr.size == var_stack->var_arr.capacity){
        var_stack->var_arr.capacity *= 2;

        char **new_var_arr = (char **)realloc(var_stack->var_arr.var_arr, 
                                              var_stack->var_arr.capacity * sizeof(char *));
        if (!new_var_arr) return;

        var_stack->var_arr.var_arr = new_var_arr;
    }
}

void push_var_stack(var_stack_t *var_arr){
    var_arr->stack_level.stack_level[var_arr->stack_level.size] = var_arr->var_arr.size;
    var_arr->stack_level.size++;

    if (var_arr->stack_level.size == var_arr->stack_level.capacity){
        var_arr->stack_level.capacity *= 2;

        size_t *new_stack_level = (size_t *)realloc(var_arr->stack_level.stack_level,
                                                    var_arr->stack_level.capacity * sizeof(size_t));
        if (!new_stack_level) return;

        var_arr->stack_level.stack_level = new_stack_level;
    }
}

int find_elem_var_stack(var_stack_t *var_stack, char *var_name){
    for (size_t pos_var_stack = 0; pos_var_stack < var_stack->var_arr.size; pos_var_stack++){
        if (strcmp(var_stack->var_arr.var_arr[pos_var_stack], var_name) == 0){
            return (int)pos_var_stack;
        }
    }

    return -1;
}

void pop_var_stack(var_stack_t *var_stack){
    var_stack->stack_level.size--;

    var_stack->var_arr.size = var_stack->stack_level.stack_level[var_stack->stack_level.size];
}

void destroy_var_stack(var_stack_t *var_stack){
    free(var_stack->var_arr.var_arr);
    free(var_stack->stack_level.stack_level);

    free(var_stack);
}