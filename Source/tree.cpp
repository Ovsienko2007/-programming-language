#include "tree.h"

node_t *create_num_node(double value, var_arr_t *var_arr) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));
    new_node->var_arr = var_arr;

    new_node->type    = NUM;
    new_node->val.num = value;
    return new_node;
}

node_t *create_var_node(char *var_name, var_arr_t *var_arr) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));
    new_node->var_arr = var_arr;

    new_node->type    = VAR;
    new_node->val.var = var_name;

    return new_node;
}

node_t *create_op_node(op_t operation, node_t *left, node_t *right, var_arr_t *var_arr) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));
    new_node->var_arr = var_arr;

    new_node->type       = OP;
    new_node->val.op     = operation;
    new_node->left_node  = left;
    new_node->right_node = right;

    return new_node;
}

void destroy_expression(expression_t expression){
    if (expression->left_node) {
        destroy_expression(expression->left_node);
    }
    if (expression->right_node) {
        destroy_expression(expression->right_node);
    }
    if (expression->else_node) {
        destroy_expression(expression->else_node);
    }

    free(expression);

    return;
}

static const int min_var_arr_capcity = 16;

var_arr_t init_var_arr(){
    var_arr_t var_arr ={};

    var_arr.capacity = min_var_arr_capcity;
    var_arr.var_arr  = (var_t *)calloc(min_var_arr_capcity, sizeof(var_t));

    return var_arr;
}

void add_var_to_var_arr(var_arr_t *var_arr, var_t new_var){
    var_arr->var_arr[var_arr->size] = new_var;
    var_arr->size++;

    if (var_arr->size == var_arr->capacity){
        var_arr->capacity *= 2;
        var_t *new_var_arr = (var_t *)realloc(var_arr->var_arr, var_arr->capacity);

        if (new_var_arr) return;

        var_arr->var_arr = new_var_arr;
    }
}

// TODO remake
//static expression_t convolution_of_constants(expression_t expression, bool *no_changes){
//    if (expression->left && expression->left->type == OP){
//        expression->left = convolution_of_constants(expression->left, no_changes);
//    }
//
//    if (expression->right && expression->right->type == OP){
//        expression->right = convolution_of_constants(expression->right, no_changes);
//    }
//
//    if (expression->type == OP &&
//        expression->left->type == NUM && 
//       (expression->right == nullptr || expression->right->type == NUM)){
//        for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
//            if (op_list[pos_op_list].op == expression->val.op){
//                *no_changes = false;
//
//                double val1 = expression->left->val.num;
//                double val2 = expression->right ? expression->right->val.num : 0;
//
//                destroy_expression(expression);
//
//                return create_num_node(op_list[pos_op_list].op_func(val1, val2));
//            }
//        }
//    }
//
//    return expression;
//}

double add_func   (double par1,                  double par2) { return par1 + par2            ; }
double sub_func   (double par1,                  double par2) { return par1 - par2            ; }
double mul_func   (double par1,                  double par2) { return par1 * par2            ; }
double div_func   (double par1,                  double par2) { return par1 / par2            ; }
double pow_func   (double par1,                  double par2) { return pow(par1, par2)        ; }
double log_func   (double par1,                  double par2) { return log(par1) / log(par2)  ; }
double ln_func    (double par1, [[maybe_unused]] double par2) { return log(par1)              ; }
double sin_func   (double par1, [[maybe_unused]] double par2) { return sin(par1)              ; }
double cos_func   (double par1, [[maybe_unused]] double par2) { return cos(par1)              ; }
double TG_func    (double par1, [[maybe_unused]] double par2) { return sin(par1) / cos(par1)  ; }
double ctg_func   (double par1, [[maybe_unused]] double par2) { return cos(par1) / sin(par1)  ; }
double arcsin_func(double par1, [[maybe_unused]] double par2) { return asin(par1)             ; }
double arccos_func(double par1, [[maybe_unused]] double par2) { return acos(par1)             ; }
double arctg_func (double par1, [[maybe_unused]] double par2) { return atan(par1)             ; }
double arcctg_func(double par1, [[maybe_unused]] double par2) { return M_PI_2 - atan(par1)    ; }
double sh_func    (double par1, [[maybe_unused]] double par2) { return sinh(par1)             ; }
double ch_func    (double par1, [[maybe_unused]] double par2) { return cosh(par1)             ; }
double th_func    (double par1, [[maybe_unused]] double par2) { return tanh(par1)             ; }
double cth_func   (double par1, [[maybe_unused]] double par2) { return cosh(par1) / sinh(par1); }
double exp_func   (double par1, [[maybe_unused]] double par2) { return exp(par1)              ; }

double is_e_func  (double par1, double par2) { return doubles_are_equal(par1, par2);  }
double is_ne_func (double par1, double par2) { return !doubles_are_equal(par1, par2); }
double is_b_func  (double par1, double par2) { return par1 <  par2; }
double is_be_func (double par1, double par2) { return par1 <= par2; }
double is_a_func  (double par1, double par2) { return par1 >  par2; }
double is_ae_func (double par1, double par2) { return par1 >= par2; }

double and_func   (double par1, double par2) {return par1 * par2;};
double or_func    (double par1, double par2) {return par1 + par2;};

bool doubles_are_equal(double par1, double par2){
    return fabs(par1 - par2) < precision;
}