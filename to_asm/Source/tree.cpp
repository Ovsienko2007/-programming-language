#include "tree.h"

node_t *create_num_node(int value) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));

    new_node->type    = NUM;
    new_node->val.num = value;
    return new_node;
}

node_t *create_var_node(char *var_name) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));

    new_node->type    = VAR;
    new_node->val.var = var_name;

    return new_node;
}

node_t *create_node(op_t operation, node_t *left, node_t *right){
    node_t *new_node     = (node_t *)calloc(1, sizeof(node_t));
    new_node->type       = OP;
    new_node->val.op     = operation;
    new_node->left_node  = left;
    new_node->right_node = right;

    return new_node;
}

node_t *create_op_node(op_t operation, node_t *left, node_t *right, node_t *else_node){
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));

    new_node->type       = OP;
    new_node->val.op     = operation;
    new_node->left_node  = left;

    if (operation == IF || operation == WHILE){
        new_node->right_node = create_node(ELSE, right, else_node);
    }
    else{
        new_node->right_node = right;
    }

    if (operation == IF || operation == WHILE || operation == ASSIGN){
        return create_node(CONNECTING_NODE, new_node);
    }

    return new_node;
}

void destroy_tree(code_tree_t tree){
    if (!tree) return;

    destroy_tree(tree->left_node);

    destroy_tree(tree->right_node);

    if (tree->type == VAR) {
        free(tree->val.var);
    }

    free(tree);
}