#ifndef AST_H
#define AST_H

/*
 * #include "vector.h"
 */

struct ast_cmd
{
    // liste de string OU vector si opti
    char **data = NULL;
};

struct ast_list
{
   struct ast_list *next;
   struct ast_cmd *cmd;
};

struct ast_if
{
    int is_list;
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
    enum ast-type type;
    union ast_union data;
};

#endif /* AST_H */
