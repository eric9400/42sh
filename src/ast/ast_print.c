#include "ast_print.h"

#include <stdio.h>
#include <stdlib.h>

static void print_tab(int tab)
{
    for (int t = 0; t < tab; t++)
        printf("    ");
}

void pretty_print(struct ast *tree, int tab)
{
    print_tab(tab);
    if (!tree)
        printf("NULL");
    else if (tree->type == AST_IF)
    {
        printf("if (");
        pretty_print(tree->data->ast_if->condition, tab + 1);
        printf("); then\n");
        pretty_print(tree->data->ast_if->then, tab + 1);
        printf("\n");
        print_tab(tab);
        printf("else\n");
        pretty_print(tree->data->ast_if->else_body, tab + 1);
#include "ast_init.h"
#include "ast_print.h"
        printf("\n");
        print_tab(tab);
        printf("fi;\n");
    }
    else if (tree->type == AST_CMD)
    {
        printf("commande [");
        vector_print(tree->data->ast_cmd->arg);
        printf("]");
    }
    else if (tree->type == AST_LIST)
    {
        printf("list \n");
        print_tab(tab);
        printf("{\n");
        pretty_print(tree->data->ast_list->cmd_if[0], tab + 1);
        for (size_t i = 1; i < tree->data->ast_list->size; i++)
        {
            printf(";\n");
            pretty_print(tree->data->ast_list->cmd_if[i], tab + 1);
        }
        printf("\n");
        print_tab(tab);
        printf("}");
    }
    // ADD NEW AST PRINT HERE
}

void ugly_print(struct ast *tree)
{
    if (!tree)
        printf("NULL");
    if (ast->type == AST_IF)
        print_if(ast);
    else if (ast->type == AST_WHILE)
        print_while(ast);
    else if (ast->type == AST_FOR)
        print_for(ast);
    else if(ast->type == AST_UNTIL)
        print_until(ast);
    else if (ast->type == AST_LIST)
        print_list(ast);
    else if (ast->type == AST_CMD)
        print_cmd(ast);
    else if (ast->type == AST_PREFIX)
        print_prefix(ast);
    else if (ast->type == AST_REDIR)
        print_redir(ast);
    else if (ast->type == AST_SP_CMD)
        print_sp_cmd(ast);
    else if (ast->type == AST_ELEMENT)
        print_element(ast);
    else if (ast->type == AST_AND || ast->type == AST_OR)
        print_ast_tree(ast);
    else if (ast->type == AST_PIPE)
        print_ast_tree_junior(ast);
    else if (ast->type == AST_NOT)
        print_not(ast);
}

static void print_if(struct ast *ast)
{
    printf("if ");
    ugly_print(tree->data->ast_if->condition);
    printf("then ");
    ugly_print(tree->data->ast_if->then);
    printf("else ");
    ugly_print(tree->data->ast_if->else_body);
    printf("fi ");
}

static void print_list(struct ast *ast)
{
    ugly_print(tree->data->ast_list->cmd_if[0]);
    for (size_t i = 1; i < tree->data->ast_list->size; i++)
    {
        printf("; ");
        ugly_print(tree->data->ast_list->cmd_if[i]);
    }
}

static void print_cmd(struct ast *ast)
{
    struct vector *v = tree->data->ast_cmd->arg;
    if (v->size == 0)
        return;
    if (v->size == 1)
    {
        printf("%s ", v->data[0]);
        return;
    }
    for (size_t i = 0; i < v->size - 1; i++)
        printf("%s ", v->data[i]);
}

static void print_for(struct ast *ast)
{
    //TODO
}

static void print_while(struct ast *ast)
{
    //TODO
}

static void print_until(struct ast *ast)
{
    //TODO
}

static void print_and(struct ast *ast)
{
    //TODO
}

static void print_or(struct ast *ast)
{
    //TODO
}

static void print_not(struct ast *ast)
{
    //TODO
}

static void print_sp_cmd(struct ast *ast)
{
    printf("sp_cmd ");
    printf("size_prefix : %lu ", ast->data->ast_sp_cmd->size_prefix);
    printf("size_element : %lu ", ast->data->ast_sp_cmd->size_element);
    for (size_t i = 0; i < ast->data->ast_sp_cmd->size_prefix; i++)
    {
        ugly_print(ast->data->ast_sp_cmd->cmd_prefix[i]);
        printf("\n");
    }
    if (ast->data->ast_sp_cmd->word)
        printf("%s ", ast->data->ast_sp_cmd->word);
    for (size_t i = 0; i < ast->data->ast_sp_cmd->size_element; i++)
    {
        ugly_print(ast->data->ast_sp_cmd->cmd_element[i]);
        printf("\n");
    }
}

static void print_sh_cmd(struct ast *ast)
{
    printf("sh_cmd ");
    printf("size_redir : %lu ", ast->data->ast_sh_cmd->size_redir);
    if (ast->data->ast_sh_cmd->cmd)
    {
        printf("command ");
        ugly_print(ast->data->ast_sh_cmd->cmd);
        printf("\n");
    }
    for (size_t i = 0; i < ast->data->ast_sh_cmd->size_redir; i++)
    {
        ugly_print(ast->data->ast_sh_cmd->redir[i]);
        printf("\n");
    }
}


static void print_element(struct ast *ast)
{
    printf("element ");
    if (ast->data->ast_element->word)
        printf("%s ", ast->data->ast_element->word);
    if (ast->data->ast_element->redir)
        ugly_print(ast->data->ast_element->redir);
}

static void print_prefix(struct ast *ast)
{
    printf("prefix ");
    if (ast->data->ast_prefix->assign_word)
        printf("%s ", ast->data->ast_prefix->assign_word);
    if (ast->data->ast_prefix->redir)
        ugly_print(ast->data->ast_prefix->redir);
}

static void print_redirect(struct ast *ast)
{
    if (ast->data->ast_redir->io_number)
        printf("%s ", ast->data->ast_redir->io_number); 
    printf("redirection ");
    if (ast->data->ast_redir->exit_file)
        printf("%s ", ast->data->ast_redir->exit_file);
}

static void print_pipe(struct ast *ast)
{
    
}

// ADD NEW AST PRINT HERE
