#include "tree.h"

int main(int argc, char**argv){ 
    const char *source_name;
    const char *dest_name;

    get_file_name(argc, argv, &source_name, &dest_name);

    tokens_arr_t tokens_arr = analysis(source_name);
    printf("lexical analysis completed\n");

    code_tree_t  expr       = get_general(tokens_arr);
    DUMP(expr);

    print_tree(expr, dest_name);

    printf("1233");

    destroy_expression(expr);
    destroy_tokens_arr(tokens_arr);
}