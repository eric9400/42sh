#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

/*
 *convert any type of ast into a general ast
 */
struct ast *convert_node_ast(enum ast_type type, void *node)
{
    struct ast *ast_node = calloc(1, sizeof(struct ast));
    ast_node->data = calloc(1, sizeof(union ast_union));
    ast_node->type = type;

    if (type == AST_LIST)
        ast_node->data->ast_list = (struct ast_list *)node;
    if (type == AST_CMD)
        ast_node->data->ast_cmd = (struct ast_cmd *)node;
    if (type == AST_IF)
        ast_node->data->ast_if = (struct ast_if *)node;
    // ADD NEW AST CONVERT HERE

    return ast_node;
}

void add_to_list(struct ast_list *list, struct ast *node)
{
    list->cmd_if[list->size] = node;
    list->size++;
}