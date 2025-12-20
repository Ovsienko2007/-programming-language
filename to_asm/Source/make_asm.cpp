#include "tree.h"

void print_node(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack);

void make_asm_file(const char *dest_name, code_tree_t tree){
    assert(dest_name);
    assert(tree);

    int free_label = 0;

    FILE *outstream = fopen(dest_name, "w");

    bool error = false;

    var_stack_t *var_stack = init_var_stack();
    push_var_stack(var_stack);

    print_node(outstream, tree, &free_label, &error, var_stack);

    if (error) printf("ERROR\n");

    destroy_var_stack(var_stack);

    fprintf(outstream, "HALT\n");

    fclose(outstream);
}

static void print_while       (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack);
static void print_if          (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack);
static void print_assign      (FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack);
static void print_var         (FILE *outstream, node_t *tree,                  bool *error, var_stack_t *var_stack);
static void print_line_of_pars(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack);

void print_node(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack){
    if (tree->type == OP){
        switch (tree->val.op){
            case IF:
                print_if(outstream, tree, free_label, error, var_stack);
                break;
            case WHILE:
                print_while(outstream, tree, free_label, error, var_stack);
                break;
            case ASSIGN:
                print_assign(outstream, tree, free_label, error, var_stack);
                break;
            case PRINT:
                print_line_of_pars(outstream, tree->left_node, free_label, error, var_stack);
                fprintf(outstream, "OUT\n");
                break;
            case INPUT:
                fprintf(outstream, "IN\n");
                break;
            case ADD  : case SUB  : case MUL  : case DIV  :
            case POW  : case IS_E : case IS_NE: case IS_BE:
            case IS_B : case IS_AE: case IS_A : case AND  : 
            case OR   : case SQRT :
                if (tree->left_node) print_node(outstream, tree->left_node, free_label, error, var_stack);

                if (tree->right_node) print_node(outstream, tree->right_node, free_label, error, var_stack);

                for (size_t check_num = 0; check_num < op_list_size; check_num++){
                    if (op_list[check_num].op == tree->val.op){
                        fprintf(outstream, "%s\n", op_list[check_num].str_op);
                    }
                }
                break;
            case CONNECTING_NODE:
                if (tree->left_node)  print_node(outstream, tree->left_node , free_label, error, var_stack);

                if (tree->right_node) print_node(outstream, tree->right_node, free_label, error, var_stack);

                break;
            case ELSE:
                printf("else?\n");
                break;
            case FUNC: case RETURN:
            default:
                *error = true;
                break;
        }
    }
    else if(tree->type == NUM){
        fprintf(outstream, "PUSH %d\n", tree->val.num);
    }
    else{
        print_var(outstream, tree, error, var_stack);
    }

    if (*error) return;
}

static void print_line_of_pars(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack){
    if (tree == nullptr) return;

    if (!(tree->type   == OP &&
          tree->val.op == CONNECTING_NODE) ||
        !tree->left_node){
            *error = true;
            return;
    }

    //last_elems
    print_line_of_pars(outstream, tree->right_node, free_label, error, var_stack);

    //current elem
    print_node(outstream, tree->left_node, free_label, error, var_stack);
}

static void print_var(FILE *outstream, node_t *tree, bool *error, var_stack_t *var_stack){
    int pos = find_elem_var_stack(var_stack, tree->val.var);

    if (pos == -1){
        *error = true;
        return;
    }

    fprintf(outstream, "PUSH %d\n", pos);
    fprintf(outstream, "POP  RBX\n"
                       "PUSH [RBX]\n");
}

static void print_assign(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack){
    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->left_node  ||
         tree->left_node->type != VAR){
            *error = true;
    }

    //source
    print_node(outstream, tree->right_node, free_label, error, var_stack);

    // destination
    int pos = find_elem_var_stack(var_stack, tree->left_node->val.var);


    if (pos == -1){
        pos = (int)var_stack->var_arr.size;

        add_elem_to_var_stack(var_stack, tree->left_node->val.var);
    }
    
    fprintf(outstream, "PUSH %d  ; %s <- %d\n", pos, tree->left_node->val.var, pos);

    fprintf(outstream, "POP  RBX\n"
                       "POP [RBX]\n");
}

static void print_if(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack){
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
    print_node(outstream, tree->left_node, free_label, error, var_stack);

    fprintf(outstream, "PUSH 0\n");
    fprintf(outstream, "JE :%d\n", start_else);

    // main
    print_node(outstream, tree->right_node->left_node, free_label, error, var_stack);

    fprintf(outstream, "JMP :%d\n", end_construct);
    fprintf(outstream, ":%d\n"    , start_else);

    // else
    if (tree->right_node->right_node){
        print_node(outstream, tree->right_node->right_node, free_label, error, var_stack);
    }

    // end
    fprintf(outstream, ":%d\n"    , end_construct);

    pop_var_stack(var_stack);
}
/**
для break передавать параметр далее вглубину обычным параметром, чтобы была поддержка вложенного говна!!!!
можно накинуть сверху exit через HALT 
 */
static void print_while(FILE *outstream, node_t *tree, int *free_label, bool *error, var_stack_t *var_stack){
    push_var_stack(var_stack);

    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->right_node->left_node){
            *error = true;
    }

    int end_construct = *free_label;
    int start_main    = *free_label + 1;
    int start_else    = *free_label + 2;
    *free_label += 3;

    // 1 condition
    print_node(outstream, tree->left_node, free_label, error, var_stack);

    fprintf(outstream, "PUSH 0\n");
    fprintf(outstream, "JE :%d\n", start_else);

    // start main
    fprintf(outstream, ":%d\n", start_main);

    // other conditions
    print_node(outstream, tree->left_node, free_label, error, var_stack);

    fprintf(outstream, "PUSH 0\n");
    fprintf(outstream, "JE :%d\n", end_construct);

    // main
    print_node(outstream, tree->right_node->left_node, free_label, error, var_stack);

    fprintf(outstream, "JMP :%d\n", start_main);

    // else
    fprintf(outstream, ":%d\n"    , start_else);

    if (tree->right_node->right_node){
        print_node(outstream, tree->right_node->right_node, free_label, error, var_stack);
    }

    // end
    fprintf(outstream, ":%d\n"    , end_construct);


    pop_var_stack(var_stack);
}