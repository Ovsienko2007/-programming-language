#include "tree.h"

static void print_nodes(FILE *out_stream, code_tree_t tree);

void print_tree(code_tree_t tree, const char *dest_name){
    assert(tree);
    assert(dest_name);

    FILE *out_stream = fopen(dest_name, "w");
    if (!out_stream) return;

    print_nodes(out_stream, tree);

    fclose(out_stream);
}

static void print_nodes(FILE *out_stream, code_tree_t tree){
    assert(out_stream);

    if (!tree){
        fprintf(out_stream, "nil");
        return;
    }

    fprintf(out_stream, "(");

    switch (tree->type){
        case OP:
            for (size_t pos_op_list = 0; pos_op_list < op_list_size; pos_op_list++){
                if (op_list[pos_op_list].op == tree->val.op){
                    fprintf(out_stream, "%s ", op_list[pos_op_list].for_result_tree);
                }
            }

            break;
        case VAR:
            fprintf(out_stream, "%s ", tree->val.var);
            break;
        case NUM:
            fprintf(out_stream, "%d ", tree->val.num);
            break;
        default:
            break;
    }

    print_nodes(out_stream , tree->left_node);

    fprintf(out_stream, " ");

    print_nodes(out_stream, tree->right_node);

    fprintf(out_stream, ")");
}