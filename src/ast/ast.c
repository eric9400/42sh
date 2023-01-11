#include "ast.h"

#include <stdio.h>
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

// ADD NEW AST INIT HERE

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
        pretty_print(tree->data->ast_if->condition, 0);
        printf("); then\n");
        pretty_print(tree->data->ast_if->then, tab + 1);
        printf("\n");
        print_tab(tab);
        printf("else\n");
        pretty_print(tree->data->ast_if->else_body, tab + 1);
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
        printf("list \n{\n");
        pretty_print(tree->data->ast_list->cmd_if[0], tab + 1);
        for (size_t i = 1; i < tree->data->ast_list->size; i++)
        {
            printf(";\n");
            pretty_print(tree->data->ast_list->cmd_if[i], tab + 1);
        }
        printf("\n}");
    }
    // ADD NEW AST PRINT HERE
}

void ugly_print(struct ast *tree)
{
    if (!tree)
        printf("NULL");
    else if (tree->type == AST_IF)
    {
        printf("if ");
        ugly_print(tree->data->ast_if->condition);
        printf("then ");
        ugly_print(tree->data->ast_if->then);
        printf("else ");
        ugly_print(tree->data->ast_if->else_body);
        printf("fi ");
    }
    else if (tree->type == AST_CMD)
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
    else if (tree->type == AST_LIST)
    {
        ugly_print(tree->data->ast_list->cmd_if[0]);
        for (size_t i = 1; i < tree->data->ast_list->size; i++)
        {
            printf("; ");
            ugly_print(tree->data->ast_list->cmd_if[i]);
        }
    }
}