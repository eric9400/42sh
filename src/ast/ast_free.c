#include "ast_free.h"

#include <stdio.h>
#include <stdlib.h>

static void free_list2(struct ast_list *list)
{
    for (size_t i = 0; i < list->size; i++)
        free_node(list->cmd_if[i]);
    free(list->cmd_if);
    free(list);
}

// 8 lines
static void free_if(struct ast *ast)
{
    if (ast->data->ast_if->condition)
        free_node(ast->data->ast_if->condition);
    if (ast->data->ast_if->then)
        free_node(ast->data->ast_if->then);
    if (ast->data->ast_if->else_body)
        free_node(ast->data->ast_if->else_body);
    free_list2(ast->data->ast_if->redir);
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
    free_list2(ast->data->ast_cmd->redir);
    free(ast->data->ast_cmd);
}

static void free_redir(struct ast *ast)
{
    if (ast->data->ast_redir->exit_file)
        free(ast->data->ast_redir->exit_file);
    free(ast->data->ast_redir);
}

// 6 lines
static void free_while(struct ast *ast)
{
    if (ast->data->ast_while->condition)
        free_node(ast->data->ast_while->condition);
    if (ast->data->ast_while->while_body)
        free_node(ast->data->ast_while->while_body);
    free_list2(ast->data->ast_while->redir);
    free(ast->data->ast_while);
}

// 6 lines
static void free_until(struct ast *ast)
{
    if (ast->data->ast_until->condition)
        free_node(ast->data->ast_until->condition);
    if (ast->data->ast_until->until_body)
        free_node(ast->data->ast_until->until_body);
    free_list2(ast->data->ast_until->redir);
    free(ast->data->ast_until);
}

// 8 lines
static void free_for(struct ast *ast)
{
    if (ast->data->ast_for->arg)
        vector_destroy(ast->data->ast_for->arg);
    if (ast->data->ast_for->for_list)
        free_node(ast->data->ast_for->for_list);
    if (ast->data->ast_for->var)
        free(ast->data->ast_for->var);
    free_list2(ast->data->ast_for->redir);
    free(ast->data->ast_for);
}

static void free_not(struct ast *ast)
{
    if (ast->data->ast_not->node)
        free_node(ast->data->ast_not->node);
    free(ast->data->ast_not);
}

// 8 lines
static void free_and_or(struct ast *ast)
{
    if (ast->type == AST_AND)
    {
        free_node(ast->data->ast_and->left);
        free_node(ast->data->ast_and->right);
        free(ast->data->ast_and);
    }
    else
    {
        free_node(ast->data->ast_or->left);
        free_node(ast->data->ast_or->right);
        free(ast->data->ast_or);
    }
}

static void free_pipe(struct ast *pipe)
{
    if (pipe->data->ast_pipe->left)
        free_node(pipe->data->ast_pipe->left);
    if (pipe->data->ast_pipe->right)
        free_node(pipe->data->ast_pipe->right);
    free(pipe->data->ast_pipe);
}

static void free_subshell(struct ast *ast)
{
    free_node(ast->data->ast_subshell->sub);
    free(ast->data->ast_subshell);
}

// 8 lines
void free_func(struct ast *ast, int force)
{
    free_node(ast->data->ast_func->func); 
    free_list2(ast->data->ast_func->redir);
    free(ast->data->ast_func->name);
    free(ast->data->ast_func);
    free(ast->data);
    ast->data = NULL;
    if (force)
        free_node(ast);
}

// 26 lines
void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == AST_FUNC)
    {
        if (ast->data == NULL)
            free(ast);
        return;
    }
    if (ast->type == AST_IF)
        free_if(ast);
    else if (ast->type == AST_WHILE)
        free_while(ast);
    else if (ast->type == AST_FOR)
        free_for(ast);
    else if (ast->type == AST_UNTIL)
        free_until(ast);
    else if (ast->type == AST_LIST)
        free_list(ast);
    else if (ast->type == AST_CMD)
        free_cmd(ast);
    else if (ast->type == AST_REDIR)
        free_redir(ast);
    else if (ast->type == AST_AND || ast->type == AST_OR)
        free_and_or(ast);
    else if (ast->type == AST_PIPE)
        free_pipe(ast);
    else if (ast->type == AST_NOT)
        free_not(ast);
    else if (ast->type == AST_SUBSHELL)
        free_subshell(ast);

    free(ast->data);
    free(ast);
}

// ADD NEW AST FREE HERE
