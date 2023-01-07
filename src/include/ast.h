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
    size_t size;
    struct ast **cmd_if;
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

struct ast_cmd *init_cmd(void);
struct ast_list *init_list(void);
struct ast_if *init_if(void);

void free_cmd(struct ast_cmd *ast_cmd);
void free_list(struct ast_list *ast_list);
void free_if(struct ast_if *ast_if);

void add_elm_list(struct ast_list *ast_list);

#endif /* AST_H */
