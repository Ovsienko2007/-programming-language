#include "tree.h"

void print_node(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr, continue_break_labels_t continue_break_labels = {-1, -1});

void make_asm_file(const char *dest_name, code_tree_t tree){
    assert(dest_name);
    assert(tree);

    int free_label = 0;

    FILE *outstream = fopen(dest_name, "w");

    bool error = false;

    func_arr_t   func_arr  = init_func_arr();

    var_stack_t *var_stack = init_var_stack();
    push_var_stack(var_stack);

    print_node(outstream, tree, &free_label, &error, var_stack, &func_arr);

    fprintf(outstream, "\tHALT\n");

    if (error) printf("ERROR\n");

    destroy_var_stack(var_stack);
    destroy_func_arr(func_arr);

    fclose(outstream);
}

static void print_while           (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr);
static void print_if              (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr, continue_break_labels_t continue_break_labels);
static void print_assign          (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr);
static int  print_line_of_pars    (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr);
static void print_func_call       (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr);
static void print_func            (FILE *outstream, node_t *tree, int *free_label, bool *error                        , func_arr_t *func_arr);
static void print_get_param       (FILE *outstream, node_t *tree,                  bool *error, var_stack_t *var_stack);
static int  print_get_line_of_pars(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack);
static void print_var             (FILE *outstream, node_t *tree,                  bool *error, var_stack_t *var_stack);
static void print_print           (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr);

void print_node(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr, continue_break_labels_t continue_break_labels){
    if (tree->type == OP){
        switch (tree->val.op){
            case IF:
                print_if(outstream, tree, free_label, error, var_stack, func_arr, continue_break_labels);
                break;
            case WHILE:
                print_while(outstream, tree, free_label, error, var_stack, func_arr);
                break;
            case ASSIGN:
                print_assign(outstream, tree, free_label, error, var_stack, func_arr);
                break;
            case PRINT:
                print_print(outstream, tree, free_label, error, var_stack, func_arr);
                break;
            case INPUT:
                fprintf(outstream, "\tIN\n");
                break;
            case BREAK:
                fprintf(outstream, "\tJMP :%d\n", continue_break_labels.break_label);
                break;
            case CONTINUE:
                fprintf(outstream, "\tJMP :%d\n", continue_break_labels.continue_label);
                break;
            case ADD  : case SUB  : case MUL  : case DIV  :
            case POW  : case IS_E : case IS_NE: case IS_BE:
            case IS_B : case IS_AE: case IS_A : case AND  : 
            case OR   : case SQRT : case RET:
                if (tree->left_node)  print_node(outstream, tree->left_node, free_label, error, var_stack, func_arr, continue_break_labels);

                if (tree->right_node) print_node(outstream, tree->right_node, free_label, error, var_stack, func_arr, continue_break_labels);

                for (size_t check_num = 0; check_num < op_list_size; check_num++){
                    if (op_list[check_num].op == tree->val.op){
                        fprintf(outstream, "\t%s\n", op_list[check_num].for_asm);
                    }
                }

                break;
            case CONNECTING_NODE:
                if (tree->left_node)  print_node(outstream, tree->left_node , free_label, error, var_stack, func_arr, continue_break_labels);

                if (tree->right_node) print_node(outstream, tree->right_node, free_label, error, var_stack, func_arr, continue_break_labels);

                break;
            case ELSE:
                printf("else?\n");
                break;
            case FUNC: 
                if (!tree->right_node){
                    print_func_call(outstream, tree, free_label, error, var_stack, func_arr);
                }
                else {
                    print_func(outstream, tree, free_label, error, func_arr);
                }
                break;
            default:
                *error = true;
                break;
        }
    }
    else if(tree->type == NUM){
        fprintf(outstream, "\tPUSH %d\n", tree->val.num);
    }
    else{
        print_var(outstream, tree, error, var_stack);
    }

    if (*error) return;
}

static void print_print(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr){
    int num_of_pars = print_line_of_pars(outstream, tree->left_node, free_label, error, var_stack, func_arr);
    
    for (int par_pos = 0; par_pos < num_of_pars; par_pos++){
        fprintf(outstream, "\tOUT\n");
    }

    fprintf(outstream, "\tNL\n");

}

static void print_assign(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr){
    //source
    print_line_of_pars(outstream, tree->right_node, free_label, error, var_stack, func_arr);

    // destination
    print_get_line_of_pars(outstream, tree->left_node, free_label, error, var_stack);
}

static void print_func_call(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr){
    if (tree->left_node->type != OP ||
        tree->left_node->val.op != CONNECTING_NODE){
            printf("ERROR func_call");
            *error = true;
            return; 
    }

    int num_of_params = print_line_of_pars(outstream, tree->left_node->right_node, free_label, error, var_stack, func_arr);

    fprintf(outstream, "\tPUSH RAX\n");
    fprintf(outstream, "\tPUSH %zu\n", var_stack->var_arr.size);
    fprintf(outstream, "\tADD\n");
    fprintf(outstream, "\tPOP RAX\n");

    int num_of_params_correct = -1;
    int label = find_func_arr(func_arr, tree->left_node->left_node->val.var, &num_of_params_correct);

    if (num_of_params_correct != num_of_params && num_of_params_correct != -1){
        *error = true;
        return;
    }
    
    if (label == -1){
        label = *free_label;
        add_elem_func_arr(func_arr, tree->left_node->left_node->val.var, num_of_params, free_label);
    }

    fprintf(outstream, "\tCALL :%d\n", label);

    fprintf(outstream, "\tPUSH RAX\n");
    fprintf(outstream, "\tPUSH %zu\n", var_stack->var_arr.size);
    fprintf(outstream, "\tSUB\n");
    fprintf(outstream, "\tPOP RAX\n");
}

static void print_func(FILE *outstream, node_t *tree, int *free_label, bool *error, func_arr_t *func_arr){
    var_stack_t *new_var_stack = init_var_stack();

    int end_func_label = *free_label;
    (*free_label)++;

    fprintf(outstream, "\tJMP :%d\n", end_func_label);

    int num_of_params_correct = -1;
    int func_label = find_func_arr(func_arr, tree->left_node->left_node->val.var, &num_of_params_correct);

    if (func_label == -1){
        func_label = *free_label;
    }

    fprintf(outstream, ":%d\n", func_label);
    push_var_stack(new_var_stack);

    int num_of_params = print_get_line_of_pars(outstream, tree->left_node->right_node, free_label, error, new_var_stack);

    if (num_of_params_correct != -1 && num_of_params_correct != num_of_params){
        *error = true;
        return;
    }

    if (func_label == *free_label){
        fprintf(stderr, "print_func\n");
        add_elem_func_arr(func_arr, tree->left_node->left_node->val.var, num_of_params, free_label);
    }

    print_node(outstream, tree->right_node, free_label, error, new_var_stack, func_arr);

    fprintf(outstream, "\tRET\n");

    fprintf(outstream, ":%d\n", end_func_label);

    destroy_var_stack(new_var_stack);
}

static int print_get_line_of_pars(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack){
    if (tree == nullptr) return 0;

    if (!(tree->type   == OP &&
          tree->val.op == CONNECTING_NODE) ||
        !tree->left_node){
            *error = true;
            printf("LINE ERROR\n");
            return 0;
    }

    //current elem
    print_get_param(outstream, tree->left_node, error, var_stack);

    //last_elems
    return 1 + print_get_line_of_pars(outstream, tree->right_node, free_label, error, var_stack);
}

static void print_get_param(FILE *outstream, node_t *tree, bool *error, var_stack_t *var_stack){
    if (tree->type != VAR){
        *error = true;
        return;
    }

    int pos = find_elem_var_stack(var_stack, tree->val.var);

    if (pos == -1){
        pos = (int)var_stack->var_arr.size;

        add_elem_to_var_stack(var_stack, tree->val.var);
    }

    fprintf(outstream, "\tPUSH RAX\n");
    fprintf(outstream, "\tPUSH %d  ; %s <- %d\n", pos, tree->val.var, pos);
    fprintf(outstream, "\tADD\n");

    fprintf(outstream, "\tPOP  RDX\n"
                       "\tPOP [RDX]\n");
}

static int print_line_of_pars(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr){
    if (tree == nullptr) return 0;

    if (!(tree->type   == OP &&
          tree->val.op == CONNECTING_NODE) ||
        !tree->left_node){
            *error = true;
            printf("LINE ERROR\n");
            return 0;
    }

    //last_elems
    int ans = 1 + print_line_of_pars(outstream, tree->right_node, free_label, error, var_stack, func_arr);

    //current elem
    print_node(outstream, tree->left_node, free_label, error, var_stack, func_arr);

    return ans;
}

static void print_var(FILE *outstream, node_t *tree, bool *error, var_stack_t *var_stack){
    int pos = find_elem_var_stack(var_stack, tree->val.var);

    if (pos == -1){
        *error = true;
        return;
    }

    fprintf(outstream, "\tPUSH RAX\n");
    fprintf(outstream, "\tPUSH %d\n", pos);
    fprintf(outstream, "\tADD\n");
    fprintf(outstream, "\tPOP  RBX\n"
                       "\tPUSH [RBX]\n");
}

static void print_if(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr, continue_break_labels_t continue_break_labels){
    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->right_node->left_node){
            *error = true;
            return;
    }

    push_var_stack(var_stack);

    int end_construct = *free_label;
    int start_else    = *free_label + 1;
    *free_label += 2;

    // condition
    print_node(outstream, tree->left_node, free_label, error, var_stack, func_arr);

    fprintf(outstream, "\tPUSH 0\n");
    fprintf(outstream, "\tJE :%d\n", start_else);

    // main
    print_node(outstream, tree->right_node->left_node, free_label, error, var_stack, func_arr, continue_break_labels);

    fprintf(outstream, "\tJMP :%d\n", end_construct);
    fprintf(outstream, ":%d\n"    , start_else);

    // else
    if (tree->right_node->right_node){
        print_node(outstream, tree->right_node->right_node, free_label, error, var_stack, func_arr, continue_break_labels);
    }

    // end
    fprintf(outstream, ":%d\n"    , end_construct);

    pop_var_stack(var_stack);
}
/**
для break передавать параметр далее вглубину обычным параметром, чтобы была поддержка вложенного говна!!!!
можно накинуть сверху exit через HALT 
 */
static void print_while(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack, func_arr_t *func_arr){
    push_var_stack(var_stack);

    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->right_node->left_node){
            *error = true;
    }

    int end_construct = *free_label;
    int start_main    = *free_label + 1;
    int start_else    = *free_label + 2; //TODO const
    *free_label += 3;

    continue_break_labels_t new_continue_break_labels = {
        .continue_label = start_main,
        .break_label    = end_construct,
    };

    // 1 condition
    print_node(outstream, tree->left_node, free_label, error, var_stack, func_arr);

    fprintf(outstream, "\tPUSH 0\n");
    fprintf(outstream, "\tJE :%d\n", start_else);

    // start main
    fprintf(outstream, ":%d\n", start_main);

    // other conditions
    print_node(outstream, tree->left_node, free_label, error, var_stack, func_arr);

    fprintf(outstream, "\tPUSH 0\n");
    fprintf(outstream, "\tJE :%d\n", end_construct);

    // main
    print_node(outstream, tree->right_node->left_node, free_label, error, var_stack, func_arr, {start_main, end_construct});

    fprintf(outstream, "\tJMP :%d\n", start_main);

    // else
    fprintf(outstream, ":%d\n"    , start_else);

    if (tree->right_node->right_node){
        print_node(outstream, tree->right_node->right_node, free_label, error, var_stack, func_arr, new_continue_break_labels);
    }

    // end
    fprintf(outstream, ":%d\n"    , end_construct);

    pop_var_stack(var_stack);
}