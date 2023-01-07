#include "ast.h"

#include <stdlib.h>

struct ast_cmd *init_cmd(void)
{
    struct ast_cmd *ast_cmd = malloc(sizeof(struct ast_cmd));
    ast_cmd->arg = vector_init(10);
    return ast_cmd;
}
struct ast_list *init_list(void)
{
    struct ast_list *ast_list = malloc(sizeof(struct ast_list));
    ast_list->cmd_if = malloc(sizeof(struct ast *));
    return ast_list;
}
struct ast_if *init_if(void)
{
    struct ast_if *ast_if = malloc(sizeof(struct ast_if));
    ast_if->condition = NULL;
    ast_if->then = NULL;
    ast_if->else_body = NULL;
    return ast_if;
}

void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == AST_IF)
    {
        free_node(ast->data.ast_if.condition);
        free_node(ast->data.ast_if.then);
        free_node(ast->data.ast_if.else_body);
    }
    else if (ast->type == AST_LIST)
    {
        for (size_t i = 0; i < ast->data.ast_list.size; i++)
            free_node(ast->data.ast_list.cmd_if[i]);
    }
    else if (ast->type == AST_CMD)
    {
        vector_destroy(ast->data.ast_cmd.arg);
    }
    free(ast);
}

void add_elm_list(struct ast_list *ast_list)
{
    (void) ast_list;
}
