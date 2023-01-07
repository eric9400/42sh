void free_node(struct ast *ast)
{
    if (!ast)
        return;
    if (ast->type == IF)
    {
        free_node(ast->data.ast_if->condition);
        free_node(ast->data.ast_if->then);
        free_node(ast->data.ast_if->else_body);
    }
    else if (ast->type == LIST)
    {
        for (size_t i = 0; i < ast->data.ast_list->size; i++)
            free_node(ast->data.ast_list->cmd[i]);
    }
    else if (ast->type == CMD)
    {
        vector_destroy(ast->data.ast_cmd->arg);
    }
    free(ast);
}
