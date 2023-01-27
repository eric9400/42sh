#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * 30 lines
 * convert any type of ast into a general ast
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
    if (type == AST_FOR)
        ast_node->data->ast_for = (struct ast_for *)node;
    if (type == AST_WHILE)
        ast_node->data->ast_while = (struct ast_while *)node;
    if (type == AST_UNTIL)
        ast_node->data->ast_until = (struct ast_until *)node;
    if (type == AST_REDIR)
        ast_node->data->ast_redir = (struct ast_redir *)node;
    if (type == AST_AND)
        ast_node->data->ast_and = (struct ast_and *)node;
    if (type == AST_OR)
        ast_node->data->ast_or = (struct ast_or *)node;
    if (type == AST_NOT)
        ast_node->data->ast_not = (struct ast_not *)node;
    if (type == AST_PIPE)
        ast_node->data->ast_pipe = (struct ast_pipe *)node;
    if (type == AST_FUNC)
        ast_node->data->ast_func = (struct ast_func *)node;
    if (type == AST_SUBSHELL)
        ast_node->data->ast_subshell = (struct ast_subshell *)node;
    if (type == AST_CASE)
        ast_node->data->ast_case = (struct ast_case *)node;
    if (type == AST_CASE_ITEM)
        ast_node->data->ast_case_item = (struct ast_case_item *)node;
    // ADD NEW AST CONVERT HERE

    return ast_node;
}

// 5 lines
void add_to_list(struct ast_list *list, struct ast *node)
{
    list->cmd_if[list->size] = node;
    list->size++;
    if (list->size >= list->capacity)
    {
        list->capacity *= 2;
        list->cmd_if = realloc(list->cmd_if, list->capacity);
    }
}
