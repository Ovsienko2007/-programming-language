#include "tree.h"

int main(int argc, char**argv){
    const char *dest_name   = nullptr;
    const char *source_name = nullptr;

    get_file_name(argc, argv, &source_name, &dest_name);

    code_tree_t code_tree = get_tree_from_file(source_name);

    DUMP(code_tree);

    make_asm_file(dest_name, code_tree);

    if (code_tree)destroy_tree(code_tree);
}