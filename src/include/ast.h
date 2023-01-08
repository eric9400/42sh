#ifndef AST_H
#define AST_H

#include "vector.h"

enum ast_type
{
    AST_LIST,
    AST_IF,
    AST_CMD
};

struct ast_cmd
{
    struct vector *arg;
};

struct ast_list
{
    size_t size;
    size_t capacity;
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
    struct ast_cmd *ast_cmd;
    struct ast_list *ast_list;
    struct ast_if *ast_if;
};

struct ast
{
    enum ast_type type;
    union ast_union *data;
};

struct ast_cmd *init_cmd(void);
struct ast_list *init_list(size_t capacity);
struct ast_if *init_if(void);

void add_to_list(struct ast_list *ast_list, struct ast *element, int *index);

void free_node(struct ast *ast);

#endif /* AST_H */
