#include "ast_free.h"

#include <stdio.h>
#include <stdlib.h>

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
    free(ast->data);
    free(ast);
}

/*
static struct ast_for *free_if(void)
{
    //TODO
    return NULL;
}

static struct ast_while *free_list(void)
{
    //TODO
    return NULL;
}

static struct ast_until *free_cmd(void)
{
    //TODO
    return NULL;
}

static struct ast_for *free_for(void)
{
    //TODO
    return NULL;
}

static struct ast_while *free_while(void)
{
    //TODO
    return NULL;
}

static struct ast_until *free_until(void)
{
    //TODO
    return NULL;
}

static struct ast_and *free_and(void)
{
    //TODO
    return NULL;
}

static struct ast_or *free_or(void)
{
    //TODO
    return NULL;
}

static struct ast_not *free_not(void)
{
    //TODO
    return NULL;
}

static struct ast_redirect *free_redirect(void)
{
    //TODO
    return NULL;
}

static struct ast_redirect *free_pipe(void)
{
    //TODO
    return NULL;
}
*/

// ADD NEW AST FREE HERE