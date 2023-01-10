#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "builtin.h"

int execute(struct ast *ast);
int func_if(struct ast *ast);
int func_list(struct ast *ast);
int check_builtin(char **str);
int func_cmd(struct ast *ast);

int func_if(struct ast *ast)
{
    if (execute(ast->data->ast_if->condition) == 0)
        return execute(ast->data->ast_if->then);
    else
        return execute(ast->data->ast_if->else_body);
}

int func_list(struct ast *ast)
{
    size_t size = ast->data->ast_list->size;
    for (size_t i = 0; i < size; i++)
    {
        if (execute(ast->data->ast_list->cmd_if[i]) != 0)
            return 2;
    }
    return 0;
}

int check_builtin(char **str)
{
    if (!strcmp(str[0], "true"))
        return 0;
    if (!strcmp(str[0], "false"))
        return 1;
    if (!strcmp(str[0], "echo"))
        return echo(str);
    return 3;
}

int func_cmd(struct ast *ast)
{
    int code = check_builtin(ast->data->ast_cmd->arg->data);
    if (code < 3)
        return code;
    int pid = fork();
    // child
    if (!pid)
    {
        execvp(ast->data->ast_cmd->arg->data[0], ast->data->ast_cmd->arg->data);
        return 2;
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

int execute(struct ast *ast)
{
    switch(ast->type)
    {
        case AST_IF:
            return func_if(ast);
        case AST_LIST:
            return func_list(ast); 
        case AST_CMD:
            return func_cmd(ast);
        default:
            return 19;
        // ADD NEW AST EXECUTE HERE
    }
}
