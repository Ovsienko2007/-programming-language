#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <assert.h>

struct dump_position {
    const char *file;
    const char *func;
    int         line;
};

struct str_code{
    ssize_t  size;
    char    *data;
    bool     error;
    char    *start_pos;
};

str_code                 get_data(const char *file_name);
void                     destroy_strcode(str_code data);
#define DUMP(expression) show_dump(expression, dump_position {__FILE__, __FUNCTION__,  __LINE__})

enum type_node{
    OP  = 0,
    VAR = 1,
    NUM = 2,
};

enum op_t{
    ADD    = 0,
    SUB    = 1,
    MUL    = 2,
    DIV    = 3,
    POW    = 4,
    SQRT   = 5,

    IS_E   = 30,
    IS_NE  = 31,
    IS_B   = 32,
    IS_BE  = 33,
    IS_A   = 34,
    IS_AE  = 35,

    AND    = 40,
    OR     = 41,

    ASSIGN = 50,

    IF     = 61,
    WHILE  = 62,
    ELSE   = 63,

    INPUT  = 81,
    PRINT  = 82,

    FUNC   = 90,
    RET    = 92,

    CONNECTING_NODE = 100,
};

struct var_name_t{
    size_t  size;
    size_t  capacity;
    char   *name;
};

struct var_t{
    var_name_t *name;
    double data;
};

union val_node{
    int         num;
    char       *var;
    op_t        op;
};

enum error_t{
    no_err          = 0,
    unknown_var_err = 1,
    expression_err  = 2,
};

struct node_t{
    type_node  type;
    val_node   val;
    node_t    *left_node;
    node_t    *right_node;
};

typedef node_t * code_tree_t;

enum op_type_t{
    math           = 1,
    comparison     = 2,
    logical        = 3,
    conditional_op = 4,
    assign_op      = 5,
    no_type        = 7,
    bracket        = 8,
    func_type      = 9,
};

struct func_t{
    op_t         op;
    const char  *for_asm;
    const char  *for_tree;
    int          size;
    int          num_of_par;
    op_type_t    type;
};

static const func_t op_list[] = {
{ADD                , "ADD"   , "+"      , sizeof("+"      ) - 1, 2, math          },
{SUB                , "SUB"   , "-"      , sizeof("-"      ) - 1, 2, math          },
{MUL                , "MUL"   , "*"      , sizeof("*"      ) - 1, 2, math          },
{DIV                , "DIV"   , "/"      , sizeof("/"      ) - 1, 2, math          },
{POW                , "POW"   , "^"      , sizeof("^"      ) - 1, 2, math          },
{SQRT               , "SQRT"  , "sqrt"   , sizeof("sqrt"   ) - 1, 2, math          },
{IS_E               , "ISE"   , "=="     , sizeof("=="     ) - 1, 2, comparison    },
{IS_NE              , "ISNE"  , "!="     , sizeof("!="     ) - 1, 2, comparison    },
{IS_BE              , "ISBE"  , "<="     , sizeof("<="     ) - 1, 2, comparison    },
{IS_B               , "ISB"   , "<"      , sizeof("<"      ) - 1, 2, comparison    },
{IS_AE              , "ISAE"  , ">="     , sizeof(">="     ) - 1, 2, comparison    },
{IS_A               , "ISA"   , ">"      , sizeof(">"      ) - 1, 2, comparison    },
{AND                , "AND"   , "&&"     , sizeof("&&"     ) - 1, 2, logical       },
{OR                 , "OR"    , "||"     , sizeof("||"     ) - 1, 2, logical       },
{IF                 , "if"    , "if"     , sizeof("if"     ) - 1, 2, conditional_op},
{WHILE              , "while" , "while"  , sizeof("while"  ) - 1, 2, conditional_op},
{ELSE               , "else"  , "else"   , sizeof("else"   ) - 1, 2, conditional_op},
{ASSIGN             , "="     , "="      , sizeof("="      ) - 1, 2, assign_op     },
{CONNECTING_NODE    , ";"     , ";"      , sizeof(";"      ) - 1, 2, no_type       },
{INPUT              , "IN"    , "input"  , sizeof("input"  ) - 1, 0, bracket       }, 
{PRINT              , "OUT"   , "print"  , sizeof("print"  ) - 1, 0, bracket       },
{FUNC               , "FUNC"  , "FUNC"   , sizeof("FUNC"   ) - 1, 0, func_type     },
{RET                , "RET"   , "RET"    , sizeof("RET"    ) - 1, 0, func_type     },
};

static const int op_list_size = sizeof(op_list) / sizeof(op_list[0]);

struct tokens_arr_t{
    size_t   size;
    size_t   capacity;
    node_t **node_arr;
    size_t   pos;
    bool     error;
};

code_tree_t get_general(tokens_arr_t tokens_arr);
void        destroy_tree(code_tree_t tree);

node_t     *create_num_node(int value);
node_t     *create_var_node(char *var_name);
node_t     *create_op_node (op_t operation, node_t *left, node_t *right, node_t *else_node = nullptr);

code_tree_t get_tree_from_file(const char *file_name);
node_t     *create_node(op_t operation, node_t *left = nullptr, node_t *right = nullptr);
void        show_dump(code_tree_t expression, dump_position position);

void        get_file_name(int argc, char **argv, const char **source_name, const char **dest_name);

void        make_asm_file(const char *dest_name, code_tree_t tree);

struct var_arr_t{
    size_t   size;
    size_t   capacity;
    char   **var_arr;
};

struct stack_level_t{   
    size_t  size;
    size_t  capacity;
    size_t *stack_level;
};

struct var_stack_t{
    var_arr_t    var_arr;
    stack_level_t stack_level;

    size_t       free_pos;
};

var_stack_t *init_var_stack();
int          find_elem_var_stack  (var_stack_t *var_stack, char *var_name);
void         add_elem_to_var_stack(var_stack_t *var_stack, char *new_elem_name);
void         pop_var_stack        (var_stack_t *var_stack);
void         destroy_var_stack    (var_stack_t *var_stack);
void         push_var_stack       (var_stack_t *var_stack);

struct func_name_t{
    int     nam_of_pars;
    char   *func_name;
    int     label;
};

struct func_arr_t{
    func_name_t *func;
    size_t size;
    size_t capacity;
};

func_arr_t init_func_arr();
void       destroy_func_arr (func_arr_t  func_arr);
int        find_func_arr    (func_arr_t *func_arr, char *func_name, int *num_of_pars);
void       add_elem_func_arr(func_arr_t *func_arr, char *func_name, int  num_of_pars, int *free_label);

#endif