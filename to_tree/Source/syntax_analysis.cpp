#include "tree.h"

static code_tree_t get_number               (tokens_arr_t *tokens_arr);
static code_tree_t get_primary_expression   (tokens_arr_t *tokens_arr);
static code_tree_t get_expression           (tokens_arr_t *tokens_arr);
static code_tree_t get_term                 (tokens_arr_t *tokens_arr);
static code_tree_t get_pow                  (tokens_arr_t *tokens_arr);
static code_tree_t get_var                  (tokens_arr_t *tokens_arr);
static code_tree_t get_comparison_operators (tokens_arr_t *tokens_arr);
static code_tree_t get_and_operator         (tokens_arr_t *tokens_arr);
static code_tree_t get_or_operator          (tokens_arr_t *tokens_arr);
static code_tree_t get_assign               (tokens_arr_t *tokens_arr);
static code_tree_t get_if                   (tokens_arr_t *tokens_arr);
static code_tree_t get_while                (tokens_arr_t *tokens_arr);
static code_tree_t get_operators            (tokens_arr_t *tokens_arr);
static code_tree_t get_print                (tokens_arr_t *tokens_arr);

code_tree_t get_general(tokens_arr_t tokens_arr){
    code_tree_t tree  = get_operators(&tokens_arr);
    code_tree_t new_tree = tree;

    while (tokens_arr.pos < tokens_arr.size){
        new_tree->right_node = get_operators(&tokens_arr);
        new_tree = new_tree->right_node;
    }
    
    return tree;
}

static code_tree_t get_operators(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *node = nullptr;

    if (tokens_arr->node_arr[tokens_arr->pos]->type == OP){
        if (tokens_arr->node_arr[tokens_arr->pos]->val.op == ELSE ||
            tokens_arr->node_arr[tokens_arr->pos]->val.op == RIGHT_CURLY_BRACKET){
                return nullptr;
        }

        if (tokens_arr->node_arr[tokens_arr->pos]->val.op == IF){
            node = get_if(tokens_arr);
        }
        else if (tokens_arr->node_arr[tokens_arr->pos]->val.op == WHILE){
            node = get_while(tokens_arr);
        }
        else if (tokens_arr->node_arr[tokens_arr->pos]->val.op == PRINT){
            node = get_print(tokens_arr);
        }
        else if (tokens_arr->node_arr[tokens_arr->pos]->val.op == INPUT){
            node = create_node(CONNECTING_NODE, tokens_arr->node_arr[tokens_arr->pos]);
            tokens_arr->pos++;
        }
        else if (tokens_arr->node_arr[tokens_arr->pos]->val.op == LEFT_CURLY_BRACKET){
            tokens_arr->pos++;
            node_t *new_node = nullptr;

            node = new_node = get_operators(tokens_arr);

            while ((new_node->right_node = get_operators(tokens_arr)) != nullptr){
                new_node = new_node->right_node;
            }
            if (tokens_arr->node_arr[tokens_arr->pos]->val.op != RIGHT_CURLY_BRACKET){
                tokens_arr->error = true;
            }
            tokens_arr->pos++;
        }
    }
    else {
        node = get_assign(tokens_arr);
    }

    return node;
}

static code_tree_t get_print(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
    tokens_arr->pos++;

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op != LEFT_BRACKET){
            tokens_arr->error = true;
    }
    tokens_arr->pos++;

    operation_node->left_node = get_or_operator(tokens_arr);

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op != RIGHT_BRACKET){
            tokens_arr->error = true;
    }
    tokens_arr->pos++;

    return create_node(CONNECTING_NODE, operation_node);
}

static code_tree_t get_if(tokens_arr_t *tokens_arr){

    assert(tokens_arr);

    node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
    tokens_arr->pos++;

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op != LEFT_BRACKET){
            tokens_arr->error = true;
    }
    tokens_arr->pos++;

    operation_node->left_node = get_or_operator(tokens_arr);

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op != RIGHT_BRACKET){
            tokens_arr->error = true;
    }
    tokens_arr->pos++;

    node_t *par2 = get_operators(tokens_arr);
    node_t *par3 = nullptr;
        
    if (tokens_arr->pos != tokens_arr->size &&
        tokens_arr->node_arr[tokens_arr->pos]->type   == OP   && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op == ELSE ){
            tokens_arr->pos++;
            par3 = get_operators(tokens_arr);
    }

    operation_node->right_node = create_node(ELSE, par2, par3);

    return create_node(CONNECTING_NODE, operation_node); 
}

static code_tree_t get_while(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
    tokens_arr->pos++;

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op != LEFT_BRACKET){
            tokens_arr->error = true;
    }
    tokens_arr->pos++;

    operation_node->left_node = get_or_operator(tokens_arr);

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op != RIGHT_BRACKET){
            tokens_arr->error = true;
    }
    tokens_arr->pos++;

    node_t *par2 = get_operators(tokens_arr);

    node_t *par3 = nullptr;
        
    if (tokens_arr->node_arr[tokens_arr->pos]         != NULL &&
        tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op == ELSE){
            tokens_arr->pos++;
            par3 = get_operators(tokens_arr);
    }

    operation_node->right_node = create_node(ELSE, par2, par3);

    return create_node(CONNECTING_NODE, operation_node); 
}

static code_tree_t get_assign(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *operation_node = nullptr;
    node_t *par1 = get_var(tokens_arr);

    if (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        tokens_arr->node_arr[tokens_arr->pos]->val.op == ASSIGN){
            operation_node = tokens_arr->node_arr[tokens_arr->pos];
            tokens_arr->pos++;

            operation_node->left_node  = par1;
            operation_node->right_node = get_or_operator(tokens_arr);
    }

    if (operation_node) return create_node(CONNECTING_NODE, operation_node);

    return par1;
}

static code_tree_t get_or_operator (tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par1 = get_and_operator(tokens_arr);

    while (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
           tokens_arr->node_arr[tokens_arr->pos]->val.op == OR){
        node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
        tokens_arr->pos++;

        node_t *par2 = get_and_operator(tokens_arr);

        operation_node->left_node  = par1;
        operation_node->right_node = par2;

        return operation_node;
    }

    return par1; 
}

static code_tree_t get_and_operator (tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par1 = get_comparison_operators(tokens_arr);

    while (tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
           tokens_arr->node_arr[tokens_arr->pos]->val.op == AND){
        node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
        tokens_arr->pos++;

        node_t *par2 = get_comparison_operators(tokens_arr);

        operation_node->left_node  = par1;
        operation_node->right_node = par2;

        return operation_node;
    }

    return par1; 
}

static code_tree_t get_comparison_operators (tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par1 = get_expression(tokens_arr);

    if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        (tokens_arr->node_arr[tokens_arr->pos]->val.op == IS_E  ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == IS_NE ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == IS_A  ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == IS_AE ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == IS_B  ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == IS_BE   )){
            node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
            tokens_arr->pos++;
            
            node_t *par2 = get_expression(tokens_arr);
            
            operation_node->left_node  = par1;
            operation_node->right_node = par2;

            return operation_node;
    }

    return par1;
}

static code_tree_t get_expression(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par1 = get_term(tokens_arr);

    if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        (tokens_arr->node_arr[tokens_arr->pos]->val.op == ADD ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == SUB )){
            node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
            tokens_arr->pos++;
            
            node_t *par2 = get_term(tokens_arr);
            
            operation_node->left_node  = par1;
            operation_node->right_node = par2;

            return operation_node;
    }

    return par1;
}

static code_tree_t get_term(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par1 = get_pow(tokens_arr);

    if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP  && 
        (tokens_arr->node_arr[tokens_arr->pos]->val.op == MUL ||
         tokens_arr->node_arr[tokens_arr->pos]->val.op == DIV )){
            node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
            tokens_arr->pos++;
            
            node_t *par2 = get_pow(tokens_arr);
            
            operation_node->left_node  = par1;
            operation_node->right_node = par2;

            return operation_node;
    }

    return par1;
}


static code_tree_t get_pow(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par1 = get_primary_expression(tokens_arr);

    if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        (tokens_arr->node_arr[tokens_arr->pos]->val.op == POW)){
            node_t *operation_node = tokens_arr->node_arr[tokens_arr->pos];
            tokens_arr->pos++;
            
            node_t *par2 = get_primary_expression(tokens_arr);
            
            operation_node->left_node  = par1;
            operation_node->right_node = par2;

            return operation_node;
    }

    return par1;
}

static code_tree_t get_primary_expression(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *ans = nullptr;

    
    if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        (tokens_arr->node_arr[tokens_arr->pos]->val.op == LEFT_BRACKET)){
        tokens_arr->pos++;

        ans = get_expression(tokens_arr);
        
        if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
            (tokens_arr->node_arr[tokens_arr->pos]->val.op != RIGHT_BRACKET)){
            tokens_arr->error = true;
        }

        tokens_arr->pos++;

        return ans;
    }
    else {
        return get_number(tokens_arr);
    }
}

static code_tree_t get_number(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par = nullptr;

    if ( tokens_arr->node_arr[tokens_arr->pos]->type   == OP && 
        (tokens_arr->node_arr[tokens_arr->pos]->val.op != RIGHT_BRACKET)){
            par = tokens_arr->node_arr[tokens_arr->pos];
            tokens_arr->pos++;

            return par;
    }

    if (tokens_arr->node_arr[tokens_arr->pos]->type == NUM){
         par = tokens_arr->node_arr[tokens_arr->pos];
         tokens_arr->pos++;
    } 

    if (!par) {
        par = get_var(tokens_arr);
    }

    if (!par) tokens_arr->error = true;

    return par;
}

static code_tree_t get_var(tokens_arr_t *tokens_arr){
    assert(tokens_arr);

    node_t *par = nullptr;

    if (tokens_arr->node_arr[tokens_arr->pos]->type == VAR){
         par = tokens_arr->node_arr[tokens_arr->pos];
         tokens_arr->pos++;
    }

    return par;
}

void destroy_expression(code_tree_t code_tree){
    if (code_tree->left_node ) destroy_expression(code_tree->left_node );
    if (code_tree->right_node) destroy_expression(code_tree->right_node);

    if  (code_tree->type   == OP && 
        (code_tree->val.op == CONNECTING_NODE ||
         code_tree->val.op == ELSE )){
            free(code_tree);
    }
}