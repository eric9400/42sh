#include "ast.h"

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
}

void free_cmd(struct ast_cmd *ast_cmd)
{
    vector_destroy(ast_cmd->arg);
    free(ast_cmd);
}
void free_list(struct ast_list *ast_list)
{
    //TODO
}
void free_if(struct ast_if *ast_if)
{
    //TODO
}

void add_elm_list(struct ast_list *ast_list, struct ast *node)
{
    //TODO
}