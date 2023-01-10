#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "builtin.h"

int execute(struct ast *ast, int return_value);
int func_if(struct ast *ast, int return_value);
int func_list(struct ast *ast, int return_value);
int check_builtin(char **str, int return_value);
int func_cmd(struct ast *ast, int return_value);

int func_if(struct ast *ast, int return_value)
{
    if (execute(ast->data->ast_if->condition, return_value) == 0)
        return execute(ast->data->ast_if->then, return_value);
    else
        return execute(ast->data->ast_if->else_body, return_value);
}

int func_list(struct ast *ast, int return_value)
{
    size_t size = ast->data->ast_list->size;
    for (size_t i = 0; i < size; i++)
    {
        if (execute(ast->data->ast_list->cmd_if[i], return_value) != 0)
            return 2;
    }
    return 0;
}

int check_builtin(char **str, int return_value)
{
    if (!strcmp(str[0], "true"))
        return 0;
    if (!strcmp(str[0], "false"))
        return 1;
    if (!strcmp(str[0], "echo"))
        return echo(str, return_value);
    return 3;
}

int func_cmd(struct ast *ast, int return_value)
{
    int code = check_builtin(ast->data->ast_cmd->arg->data, return_value);
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

/*
 * \brief recursively execute the ast
 * \param ast from parser
 * \return the code error
 */
int execute(struct ast *ast, int return_value)
{
    if (!ast)
        return 0;
    switch(ast->type)
    {
        case AST_IF:
            return func_if(ast, return_value);
        case AST_LIST:
            return func_list(ast, return_value); 
        case AST_CMD:
            return func_cmd(ast, return_value);
        default:
            return 19;
        // ADD NEW AST EXECUTE HERE
    }
}
