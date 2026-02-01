#include "tree.h"

void print_node(FILE *outstream, node_t *tree,  bool *error, int level);
void print_indentation(FILE *outstream, int level);

void make_asm_file(const char *dest_name, code_tree_t tree){
    assert(dest_name);
    assert(tree);

    FILE *outstream = fopen(dest_name, "w");

    bool error = false;

    print_node(outstream, tree, &error, 0);

    if (error) printf("ERROR\n");

    fclose(outstream);
}

static void print_while           (FILE *outstream, node_t *tree,  bool *error, int level);
static void print_if              (FILE *outstream, node_t *tree,  bool *error, int level);
static void print_assign          (FILE *outstream, node_t *tree,  bool *error, int level);
static int  print_line_of_pars    (FILE *outstream, node_t *tree,  bool *error, int level);
static void print_func_call       (FILE *outstream, node_t *tree,  bool *error);
static void print_func            (FILE *outstream, node_t *tree,  bool *error, int level);
static int  print_get_line_of_pars(FILE *outstream, node_t *tree,  bool *error);
static void print_var             (FILE *outstream, node_t *tree,  bool *error);
static void print_print           (FILE *outstream, node_t *tree,  bool *error, int level);

void print_node(FILE *outstream, node_t *tree, bool *error, int level){
    if (tree->type == OP){
        switch (tree->val.op){
            case IF:
                print_if(outstream, tree, error, level);
                break;
            case WHILE:
                print_while(outstream, tree, error, level);
                break;
            case ASSIGN:
                print_assign(outstream, tree, error, level);
                break;
            case PRINT:
                print_print(outstream, tree, error, level);
                break;
            case INPUT:
                fprintf(outstream, "с консоли");
                break;
            case ADD  : case SUB  : case MUL  : case DIV  :
            case POW  : case IS_E : case IS_NE: case IS_BE:
            case IS_B : case IS_AE: case IS_A : case AND  : 
            case OR   :
                if (tree->left_node) print_node(outstream, tree->left_node, error, level);

                for (size_t check_num = 0; check_num < op_list_size; check_num++){
                    if (op_list[check_num].op == tree->val.op){
                        fprintf(outstream, " %s ", op_list[check_num].for_code);
                    }
                }

                if (tree->right_node) print_node(outstream, tree->right_node, error, level);

                break;
            case SQRT:
                fprintf(outstream, "sqrt(");

                print_node(outstream, tree->left_node, error, level);

                fprintf(outstream, ")");
                break;
            case RET:
                print_indentation(outstream, level);
                fprintf(outstream, "отдай ");

                print_node(outstream, tree->left_node, error, level);

                fprintf(outstream, "\n");
                break;
            case CONNECTING_NODE:
                if (tree->left_node)  print_node(outstream, tree->left_node , error, level);

                if (tree->right_node) print_node(outstream, tree->right_node, error, level);

                break;
            case ELSE:
                printf("else?\n");
                break;
            case FUNC: 
                if (!tree->right_node){
                    print_func_call(outstream, tree, error);
                }
                else {
                    print_func(outstream, tree, error, level);
                }
                break;
            default:
                *error = true;
                break;
        }
    }
    else if(tree->type == NUM){
        fprintf(outstream, "%d", tree->val.num);
    }
    else{
        print_var(outstream, tree, error);
    }

    if (*error) return;
}

static void print_print(FILE *outstream, node_t *tree, bool *error, int level){
    print_indentation(outstream, level);
    fprintf(outstream, "Именем Омнииссии заклинаю тебя, Машина, выведи сии значения в консоль: ");

    print_line_of_pars(outstream, tree->left_node, error, 0);

    fprintf(outstream, "\n\n");
}


static void print_assign(FILE *outstream, node_t *tree,  bool *error, int level){
    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->left_node  ||
         tree->left_node->type != VAR){
            *error = true;
    }

    print_indentation(outstream, level);

    //source
    print_var(outstream, tree->left_node, error);

    fprintf(outstream, " даруй значение ");

    // destination
    print_node(outstream, tree->right_node, error, 0);

    fprintf(outstream, "\n\n");
}

static void print_func_call(FILE *outstream, node_t *tree, bool *error){
    if (tree->left_node->type != OP ||
        tree->left_node->val.op != CONNECTING_NODE ||
        !tree->left_node->left_node ||
        tree->left_node->left_node->type != VAR){
            printf("ERROR func_call");
            *error = true;
            return; 
    }

    fprintf(outstream, "%s(", tree->left_node->left_node->val.var);

    print_line_of_pars(outstream, tree->left_node->right_node, error, 0);

    fprintf(outstream, ")");
}

static void print_func(FILE *outstream, node_t *tree,  bool *error, int level){
    print_indentation(outstream, level);
    fprintf(outstream, "О, Омнииссия, даруй же Машине силы выполнить сей приказ ");

    fprintf(outstream, "%s(", tree->left_node->left_node->val.var);

    print_get_line_of_pars(outstream, tree->left_node->right_node, error);

    fprintf(outstream, "){\n");

    print_node(outstream, tree->right_node, error, level + 1);

    print_indentation(outstream, level);
    fprintf(outstream, "}\n");

}

static int print_get_line_of_pars(FILE *outstream, node_t *tree,  bool *error){
    if (tree == nullptr) return 0;

    if (!(tree->type   == OP &&
          tree->val.op == CONNECTING_NODE) ||
        !tree->left_node){
            *error = true;
            printf("LINE ERROR\n");
            return 0;
    }

    //current elem
    print_var(outstream, tree->left_node, error);

    if (tree->right_node){
        fprintf(outstream, ", ");
    }

    //last_elems
    return 1 + print_get_line_of_pars(outstream, tree->right_node, error);
}

static void print_var(FILE *outstream, node_t *tree, bool *error){
    if (tree->type != VAR){
        *error = true;
        return;
    }

    fprintf(outstream, "%s", tree->val.var);
}


static int print_line_of_pars(FILE *outstream, node_t *tree,  bool *error, int level){
    if (tree == nullptr) return 0;

    if (!(tree->type   == OP &&
          tree->val.op == CONNECTING_NODE) ||
        !tree->left_node){
            *error = true;
            printf("LINE ERROR\n");
            return 0;
    }

    //current elem
    print_node(outstream, tree->left_node, error, level);

    if (tree->right_node){
        fprintf(outstream, ", ");
    }

    //last_elems
    int ans = 1 + print_line_of_pars(outstream, tree->right_node, error, level);

    return ans;
}

static void print_if(FILE *outstream, node_t *tree,  bool *error, int level){
    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->right_node->left_node){
            *error = true;
            return;
    }

    print_indentation(outstream, level);
    fprintf(outstream, "Во имя Императора выполни если (");

    print_node(outstream, tree->left_node, error, 0);

    fprintf(outstream, "){\n");

    print_node(outstream, tree->right_node->left_node, error, level + 1);

    print_indentation(outstream, level);
    fprintf(outstream, "}\n");

    if (tree->right_node->right_node){
        print_indentation(outstream, level);
        fprintf(outstream, "В противном случае {\n");

        print_node(outstream, tree->right_node->right_node, error, level + 1);

        print_indentation(outstream, level);
        fprintf(outstream, "}\n");
        
    }

    fprintf(outstream, "\n");

    
}

static void print_while(FILE *outstream, node_t *tree,  bool *error, int level){
    if (!tree->left_node  ||
        !tree->right_node ||
        !tree->right_node->left_node){
            *error = true;
            return;
    }

    print_indentation(outstream, level);
    fprintf(outstream, "Во имя Императора выполняй пока (");

    print_node(outstream, tree->left_node, error, 0);

    fprintf(outstream, "){\n");

    print_node(outstream, tree->right_node->left_node, error, level + 1);

    if (tree->right_node->right_node){
        print_indentation(outstream, level);
        fprintf(outstream, "В противном случае {\n");

        print_node(outstream, tree->right_node->right_node, error, level + 1);
    }

    fprintf(outstream, "}\n\n");
}

void print_indentation(FILE *outstream, int level){
    for (int tub_num = 0; tub_num < level; tub_num++){
        fprintf(outstream, "\t");
    }
}