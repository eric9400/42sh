#ifndef AST_H
#define AST_H

#include "vector.h"

enum ast_type
{
    LIST,
    IF,
    CMD
};

struct ast_cmd
{
    // liste de string OU vector
    struct vector *arg;
};

struct ast_list
{
   struct ast **cmd;
};

struct ast_if
{
    struct ast *condition;
    struct ast *then;
    struct ast *else_body;
};

union ast_union
{
    struct ast_cmd ast_cmd;
    struct ast_list ast_list;
    struct ast_if ast_if;
};

struct ast
{
    enum ast_type type;
    union ast_union data;
};

#endif /* AST_H */
