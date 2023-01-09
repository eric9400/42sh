#include "ast.h"

#include <stdlib.h>

struct ast_cmd *init_cmd(void)
{
    struct ast_cmd *ast_cmd = calloc(1, sizeof(struct ast_cmd));
    ast_cmd->arg = vector_init(10);
    return ast_cmd;
}

struct ast_list *init_list(size_t capacity)
{
    struct ast_list *ast_list = calloc(1, sizeof(struct ast_list));
    ast_list->cmd_if = calloc(capacity, sizeof(struct ast *));
    ast_list->size = 0;
    ast_list->capacity = capacity;
    return ast_list;
}

struct ast_if *init_if(void)
{
    struct ast_if *ast_if = calloc(1, sizeof(struct ast_if));
    ast_if->condition = NULL;
    ast_if->then = NULL;
    ast_if->else_body = NULL;
    return ast_if;
}

//ADD NEW AST INIT HERE

/*
void add_to_list(struct ast_list *ast_list, struct ast *element, int *index)
{
    ast_list->cmd_if[*index] = element;
    ast_list->size++;
    *(index)++;
}*/

void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == AST_IF)
    {
        if (ast->data->ast_if->condition)
            free_node(ast->data->ast_if->condition);
        if (ast->data->ast_if->then)
            free_node(ast->data->ast_if->then);
        if (ast->data->ast_if->else_body)
            free_node(ast->data->ast_if->else_body);
        free(ast->data->ast_if);
    }
    else if (ast->type == AST_LIST)
    {
        for (size_t i = 0; i < ast->data->ast_list->size; i++)
            free_node(ast->data->ast_list->cmd_if[i]);
        free(ast->data->ast_list->cmd_if);
        free(ast->data->ast_list);
    }
    else if (ast->type == AST_CMD)
    {
        vector_destroy(ast->data->ast_cmd->arg);
        free(ast->data->ast_cmd);
    }
    // ADD NEW FREE AST HERE
    free(ast->data);
    free(ast);
}