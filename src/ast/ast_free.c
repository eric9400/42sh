#include "ast_free.h"

#include <stdio.h>
#include <stdlib.h>

static void free_if(struct ast *ast);
static void free_list(struct ast *ast);
static void free_cmd(struct ast *ast);
/*static void free_for(struct ast *ast);
static void free_while(struct ast *ast);
static void free_until(struct ast *ast);
static void free_and(struct ast *ast);
static void free_or(struct ast *ast);
static void free_not(struct ast *ast);
static void free_redirect(struct ast *ast);
static void free_pipe(struct ast *ast);*/

void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == AST_IF)
        free_if(ast);
    else if (ast->type == AST_LIST)
        free_list(ast);
    else if (ast->type == AST_CMD)
        free_cmd(ast);
    free(ast->data);
    free(ast);
}

static void free_if(struct ast *ast)
{
    if (ast->data->ast_if->condition)
        free_node(ast->data->ast_if->condition);
    if (ast->data->ast_if->then)
        free_node(ast->data->ast_if->then);
    if (ast->data->ast_if->else_body)
        free_node(ast->data->ast_if->else_body);
    free(ast->data->ast_if);
}

static void free_list(struct ast *ast)
{
    for (size_t i = 0; i < ast->data->ast_list->size; i++)
        free_node(ast->data->ast_list->cmd_if[i]);
    free(ast->data->ast_list->cmd_if);
    free(ast->data->ast_list);
}

static void free_cmd(struct ast *ast)
{
    vector_destroy(ast->data->ast_cmd->arg);
        free(ast->data->ast_cmd);
}
/*
static void free_for(struct ast *ast)
{
    //TODO
}

static void free_while(struct ast *ast)
{
    //TODO
}

static void free_until(struct ast *ast)
{
    //TODO
}

static void free_and(struct ast *ast)
{
    //TODO
}

static void free_or(struct ast *ast)
{
    //TODO
}

static void free_not(struct ast *ast)
{
    //TODO
}

static void free_redirect(struct ast *ast)
{
    //TODO
}

static void free_pipe(struct ast *ast)
{
    //TODO
}*/

// ADD NEW AST FREE HERE