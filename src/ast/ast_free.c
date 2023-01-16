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

static void free_redir(struct ast *ast)
{
    if (ast->data->ast_redir->exit_file)
        free(ast->data->ast_redir->exit_file);
    free(ast->data->ast_redir);
}

static void free_while(struct ast *ast)
{
    if (ast->data->ast_while->condition)
        free_node(ast->data->ast_while->condition);
    if (ast->data->ast_while->while_body)
        free_node(ast->data->ast_while->while_body);
    free(ast->data->ast_while);
}

static void free_until(struct ast *ast)
{
    if (ast->data->ast_until->condition)
        free_node(ast->data->ast_until->condition);
    if (ast->data->ast_until->until_body)
        free_node(ast->data->ast_until->until_body);
    free(ast->data->ast_until);
}

static void free_for(struct ast *ast)
{
    if (ast->data->ast_for->arg)
        vector_destroy(ast->data->ast_for->arg);
    if (ast->data->ast_for->for_list)
        free_node(ast->data->ast_for->for_list);
    if (ast->data->ast_for->var)
        free(ast->data->ast_for->var);
    free(ast->data->ast_for);
}

static void free_not(struct ast *ast)
{
    if (ast->data->ast_not->node)
        free_node(ast->data->ast_not->node);
    free(ast->data->ast_not);
}

static void free_ast_tree(struct ast *tree)
{
    while(tree)
    {
        struct ast *tmp = NULL;
        if (tree->type == AST_AND)
        {
            tmp = tree->data->ast_and->left;
            if (tree->data->ast_and->right)
                free_node(tree->data->ast_and->right);
        }
        else if (tree->type == AST_OR)
        {
            tmp = tree->data->ast_or->left;
            if (tree->data->ast_or->right)
                free_node(tree->data->ast_or->right);
        }

        if (tree)
            free_node(tree);
        tree = tmp;
    }
}

static void free_ast_tree_junior(struct ast *tree)
{
    while(tree)
    {
        struct ast *tmp = NULL;

        if (tree->type == AST_NOT)
        {
            free_node(tree);
            break;
        }

        tmp = tree->data->ast_pipe->left;
        if (tree->data->ast_pipe->right)
            free_node(tree->data->ast_pipe->right);
        if (tree)
            free_node(tree);
        if (!tmp)
            break;

        if (tmp->type == AST_PIPE)
            tree = (convert_node_ast(AST_PIPE, tmp));
        else 
            tree = (convert_node_ast(AST_NOT, tmp));
    }
}

void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == AST_IF)
        free_if(ast);
    else if (ast->type == AST_WHILE)
        free_while(ast);
    else if (ast->type == AST_FOR)
        free_for(ast);
    else if(ast->type == AST_UNTIL)
        free_until(ast);
    else if (ast->type == AST_LIST)
        free_list(ast);
    else if (ast->type == AST_CMD)
        free_cmd(ast);
    else if (ast->type == AST_REDIR)
        free_redir(ast);
    else if (ast->type == AST_AND || ast->type == AST_OR)
        free_ast_tree(ast);
    else if (ast->type == AST_PIPE)
        free_ast_tree_junior(ast);
    else if (ast->type == AST_NOT)
        free_not(ast);

    free(ast->data);
    free(ast);
}

// ADD NEW AST FREE HERE
