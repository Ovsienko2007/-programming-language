#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

struct dump_position {
    const char *file;
    const char *func;
    int         line;
};

#define DUMP(expression) show_dump(expression, dump_position {__FILE__, __FUNCTION__,  __LINE__})

enum operator_t{ // TODO
    if_op    = 0,
    while_op = 1,
    else_op  = 2,
};

enum type_node{
    OP  = 0,
    NUM = 1,
    VAR = 2,
};

enum op_t{
    ADD    = 0,
    SUB    = 1,
    MUL    = 2,
    DIV    = 3,
    POW    = 4,
    LOG    = 5,
    LN     = 6,
    SIN    = 7,
    COS    = 8,
    TG     = 9,
    CTG    = 10,
    ARCSIN = 11,
    ARCCOS = 12,
    ARCTG  = 13,
    ARCCTG = 14,
    SH     = 15,
    CH     = 16,
    TH     = 17,
    CTH    = 18,
    EXP    = 19,

    IS_E   = 30,
    IS_NE  = 31,
    IS_B   = 32,
    IS_BE  = 33,
    IS_A   = 34,
    IS_AE  = 35,

    AND    = 41,
    OR     = 42,
};

struct arr_t{
    int *data;
    int  size;
};

union data_type_t{
    double lf;
    int    num;
    arr_t  arr;
};

enum data_t{
    num = 0,
    arr = 1,
    lf  = 2,
};

struct var_name_t{
    size_t  size;
    size_t  capacity;
    char   *name;
};

struct var_t{
    var_name_t *name;
    data_t      type;
    data_type_t data;
};

union val_node{
    double      num;
    char       *var;
    op_t        op;
    operator_t  lang_op;
};

struct var_arr_t{
    size_t  size;
    size_t  capacity;
    var_t  *var_arr;
};

struct node_t{
    var_arr_t *var_arr;
    type_node  type;
    val_node   val;
    node_t    *left_node;
    node_t    *right_node;
    node_t    *else_node;
};

var_arr_t init_var_arr();
void      add_var_to_var_arr(var_arr_t *var_arr, var_t new_var);

typedef node_t * expression_t;

expression_t get_general(const char* file_name);

void         destroy_expression(expression_t expression);
node_t      *init_node(type_node type, val_node val, node_t *left, node_t *right);
node_t      *create_op_node(op_t operation, node_t *left, node_t *right, var_arr_t *var_arr);
node_t      *create_var_node(char *name_val, var_arr_t *var_arr);
node_t      *create_num_node(double value, var_arr_t *var_arr);
expression_t get_tree_from_file(const char *file_name);

void show_dump(expression_t expression, dump_position position);

typedef double (*op_func_t)(double, double); 

expression_t simplification_of_expression(expression_t expression);

const double precision = 1e-5;
bool doubles_are_equal(double par1, double par2);

double add_func   (double par1, double par2);
double sub_func   (double par1, double par2);
double mul_func   (double par1, double par2);
double div_func   (double par1, double par2);
double pow_func   (double par1, double par2);
double log_func   (double par1, double par2);
double ln_func    (double par1, double par2);
double sin_func   (double par1, double par2);
double cos_func   (double par1, double par2);
double TG_func    (double par1, double par2);
double ctg_func   (double par1, double par2);
double arcsin_func(double par1, double par2);
double arccos_func(double par1, double par2);
double arctg_func (double par1, double par2);
double arcctg_func(double par1, double par2);
double sh_func    (double par1, double par2);
double ch_func    (double par1, double par2);
double th_func    (double par1, double par2);
double cth_func   (double par1, double par2);
double exp_func   (double par1, double par2);

double is_e_func  (double par1, double par2);
double is_ne_func (double par1, double par2);
double is_b_func  (double par1, double par2);
double is_be_func (double par1, double par2);
double is_a_func  (double par1, double par2);
double is_ae_func (double par1, double par2);

double and_func   (double par1, double par2);
double or_func    (double par1, double par2);

enum op_type_t{
    math       = 1,
    comparison = 2,
    logical    = 3,
};

struct func_t{
    op_t         op;
    const char  *str_op;
    const char  *char_op;
    op_func_t    op_func;
    int          num_of_par;
    op_type_t    type;
};

static const func_t op_list[] = {
{ADD    , "ADD"   , "+"     , add_func   , 2, math},
{SUB    , "SUB"   , "-"     , sub_func   , 2, math},
{MUL    , "MUL"   , "*"     , mul_func   , 2, math},
{DIV    , "DIV"   , "/"     , div_func   , 2, math},
{POW    , "POW"   , "^"     , pow_func   , 2, math},
{LOG    , "LOG"   , "_"     , log_func   , 2, math},
{LN     , "LN"    , "ln"    , ln_func    , 1, math},
{SIN    , "SIN"   , "sin"   , sin_func   , 1, math},
{COS    , "COS"   , "cos"   , cos_func   , 1, math},
{TG     , "TG"    , "tg"    , TG_func    , 1, math},
{CTG    , "CTG"   , "ctg"   , ctg_func   , 1, math},
{ARCSIN , "ARCSIN", "arcsin", arcsin_func, 1, math},
{ARCCOS , "ARCCOS", "arccos", arccos_func, 1, math},
{ARCTG  , "ARCTG" , "arctg" , arctg_func , 1, math},
{ARCCTG , "ARCCTG", "arcctg", arcctg_func, 1, math},
{SH     , "SH"    , "sh"    , sh_func    , 1, math},
{CH     , "CH"    , "ch"    , ch_func    , 1, math},
{TH     , "TH"    , "th"    , th_func    , 1, math},
{CTH    , "CTH"   , "cth"   , cth_func   , 1, math},
{EXP    , "EXP"   , "exp"   , exp_func   , 1, math},
{IS_E   , "IS_E " , "=="    , is_e_func  , 2, comparison},
{IS_NE  , "IS_NE" , "!="    , is_ne_func , 2, comparison},
{IS_B   , "IS_B " , "<"     , is_b_func  , 2, comparison},
{IS_BE  , "IS_BE" , "<="    , is_be_func , 2, comparison},
{IS_A   , "IS_A " , ">"     , is_a_func  , 2, comparison},
{IS_AE  , "IS_AE" , ">="    , is_ae_func , 2, comparison},
{AND    , "AND"   , ""      , and_func   , 2, logical},
{OR     , "OR "   , ""      , or_func    , 2, logical},
};


#endif