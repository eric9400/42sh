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
    if (type == AST_FOR)
        ast_node->data->ast_for = (struct ast_for *)node;
    if (type == AST_WHILE)
        ast_node->data->ast_while = (struct ast_while *)node;
    if (type == AST_UNTIL)
        ast_node->data->ast_until = (struct ast_until *)node;
    if (type == AST_PREFIX)
        ast_node->data->ast_prefix = (struct ast_prefix *)node;
    if (type == AST_REDIR)
        ast_node->data->ast_redir = (struct ast_redir *)node;
    if (type == AST_ELEMENT)
        ast_node->data->ast_element = (struct ast_element *)node;
    if (type == AST_SP_CMD)
        ast_node->data->ast_sp_cmd = (struct ast_sp_cmd *)node;
    if (type == AST_SH_CMD)
        ast_node->data->ast_sh_cmd = (struct ast_sh_cmd *)node;
    if (type == AST_AND)
        ast_node->data->ast_and = (struct ast_and *)node;
    if (type == AST_OR)
        ast_node->data->ast_or = (struct ast_or *)node;
    if (type == AST_NOT)
        ast_node->data->ast_not = (struct ast_not *)node;
    if (type == AST_PIPE)
        ast_node->data->ast_pipe = (struct ast_pipe *)node;
    // ADD NEW AST CONVERT HERE

    return ast_node;
}

void add_to_list(struct ast_list *list, struct ast *node)
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
