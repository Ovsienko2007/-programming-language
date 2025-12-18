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

    LEFT_BRACKET        = 71,
    RIGHT_BRACKET       = 72,
    LEFT_CURLY_BRACKET  = 73,
    RIGHT_CURLY_BRACKET = 74,

    INPUT               = 81,
    PRINT               = 82,

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
};

struct func_t{
    op_t         op;
    const char  *str_op;
    const char  *char_op;
    int          size;
    int          num_of_par;
    op_type_t    type;
};

static const func_t op_list[] = {
{ADD                , "+"       , "+"      , sizeof("+"      ) - 1, 2, math          },
{SUB                , "-"       , "-"      , sizeof("-"      ) - 1, 2, math          },
{MUL                , "*"       , "*"      , sizeof("*"      ) - 1, 2, math          },
{DIV                , "/"       , "/"      , sizeof("/"      ) - 1, 2, math          },
{POW                , "^"       , "^"      , sizeof("^"      ) - 1, 2, math          },
{IS_E               , "=="      , "=="     , sizeof("=="     ) - 1, 2, comparison    },
{IS_NE              , "!="      , "!="     , sizeof("!="     ) - 1, 2, comparison    },
{IS_BE              , "<="      , "<="     , sizeof("<="     ) - 1, 2, comparison    },
{IS_B               , "<"       , "<"      , sizeof("<"      ) - 1, 2, comparison    },
{IS_AE              , ">="      , ">="     , sizeof(">="     ) - 1, 2, comparison    },
{IS_A               , ">"       , ">"      , sizeof(">"      ) - 1, 2, comparison    },
{AND                , "&&"      , "&&"     , sizeof("&&"     ) - 1, 2, logical       },
{OR                 , "||"      , "||"     , sizeof("||"     ) - 1, 2, logical       },
{IF                 , "if"      , "if"     , sizeof("if"     ) - 1, 2, conditional_op},
{WHILE              , "while"   , "while"  , sizeof("while"  ) - 1, 2, conditional_op},
{ELSE               , "else"    , "else"   , sizeof("else"   ) - 1, 2, conditional_op},
{ASSIGN             , "="       , "="      , sizeof("="      ) - 1, 2, assign_op     },
{CONNECTING_NODE    , ";"       , ";"      , sizeof(";"      ) - 1, 2, no_type       },
{LEFT_BRACKET       , "("       , "("      , sizeof("("      ) - 1, 0, bracket       }, 
{RIGHT_BRACKET      , ")"       , ")"      , sizeof(")"      ) - 1, 0, bracket       }, 
{LEFT_CURLY_BRACKET , "{"       , "{"      , sizeof("{"      ) - 1, 0, bracket       }, 
{RIGHT_CURLY_BRACKET, "}"       , "}"      , sizeof("}"      ) - 1, 0, bracket       }, 
{INPUT              , "input"   , "input()", sizeof("input()") - 1, 0, bracket       }, 
{PRINT              , "print"   , "print"  , sizeof("print"  ) - 1, 0, bracket       },
};

static const int op_list_size = sizeof(op_list) / sizeof(op_list[0]);

struct tokens_arr_t{
    size_t   size;
    size_t   capacity;
    node_t **node_arr;
    size_t   pos;
    bool     error;
};

tokens_arr_t analysis(const char* file_name);
void         add_elem_to_tokens_arr(tokens_arr_t *tokens_arr, node_t *new_token);
void         destroy_tokens_arr(tokens_arr_t tokens_arr);

code_tree_t get_general(tokens_arr_t tokens_arr);
void        destroy_expression(code_tree_t expression);
node_t     *create_num_node(int value);
node_t     *create_var_node(char *var_name);
node_t     *create_op_node (op_t operation, node_t *left, node_t *right, node_t *else_node = nullptr);

code_tree_t get_tree_from_file(const char *file_name);
node_t *create_node(op_t operation, node_t *left = nullptr, node_t *right = nullptr);
void    show_dump(code_tree_t expression, dump_position position);

void    get_file_name(int argc, char **argv, const char **source_name, const char **dest_name);

void    print_tree(code_tree_t tree, const char *dest_name);
#endif