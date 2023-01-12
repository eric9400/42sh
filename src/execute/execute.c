#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "builtin.h"
#include "hash_map.h"

int execute(struct ast *ast, int return_value);
static int func_if(struct ast *ast, int return_value);
static int func_list(struct ast *ast, int return_value);
static int check_builtin(char **str, int return_value);
static int func_cmd(struct ast *ast, int return_value);
/*static int func_while(struct ast *ast, int return_value);
static int func_until(struct ast *ast, int return_value);
static int func_for(struct ast *ast, int return_value);
static int func_operation(struct ast *ast, int return_value);*/


int is_still_variable(char str)
{
    return (str <= 'z' && str >= 'a') || (str <= 'Z' && str >= 'A') || (str <= '9' && str >= '0') || str == '_';
}

static void expandinho(char **str)
{
    if (!(*str))
        return;
    size_t len = strlen(*str);
    char *new = malloc(len + 1);
    int lenvar = 0;
    int single_quote = 0;
    char *hkey = NULL;
    const char *value = NULL;
    int indnew = 0;
    int copylen = len;
    for (size_t i = 0; i < len; i++, indnew++)
    {
        if ((*str)[i] == '$' && !single_quote)
        {
            while (is_still_variable((*str)[i + lenvar + 1]))
                lenvar++;
            lenvar++;
            hkey = strndup(*str + i + 1, lenvar - 1);
            value = hash_map_get(hashmap, hkey);
            free(hkey);
            if (!value)
                new = realloc(new, copylen - 1 - lenvar);
            else
            {
                copylen = copylen - lenvar + strlen(value);
                new = realloc(new, copylen + 1);
                for (size_t j = 0; j < strlen(value); j++, indnew++)
                    new[indnew] = value[j];
                indnew--;
            }
            i += lenvar - 1;
            indnew--;
        }
        else
            new[indnew] = (*str)[i];
        if ((*str)[i] == '\'')
            single_quote = !single_quote;
        lenvar = 0;
    }
    new[indnew] = '\0';
    free(*str);
    *str = new;
}
/*
static int func_while(struct ast *ast, int return_value)
{
    int res = 0;
    while ((res = execute(ast, return_value)) == 0)
        res = execute(ast->data->ast_while->while_body, return_value);
    return res;
}

static int func_until(struct ast *ast, int return_value)
{
    int res = 0;
    while ((res = execute(ast, return_value)) != 0)
        res = execute(ast->data->ast_until->until_body, return_value);
    return res;
}

static int func_for(struct ast *ast, int return_value)
{
    int res = 0;
    for (size_t i = 0; i < ast->data->ast_cmd->arg->size; i++)
    {
        hash_map_insert(hashmap, ast->data->ast_for->var, ast->data->ast_for->for_list->data->ast_cmd->arg[i]);
        res = execute(ast->data->ast_for->for_body);
    }
    return res;
}*/

static int func_if(struct ast *ast, int return_value)
{
    if (execute(ast->data->ast_if->condition, return_value) == 0)
        return execute(ast->data->ast_if->then, return_value);
    else
        return execute(ast->data->ast_if->else_body, return_value);
}

static int func_list(struct ast *ast, int return_value)
{
    size_t size = ast->data->ast_list->size - 1;

    for (size_t i = 0; i < size; i++)
        execute(ast->data->ast_list->cmd_if[i], return_value);

    // only check last return code from the command
    if (execute(ast->data->ast_list->cmd_if[size], return_value) != 0)
        return 2;
    return 0;
}

static int check_builtin(char **str, int return_value)
{
    if (!strcmp(str[0], "true"))
        return 0;
    if (!strcmp(str[0], "false"))
        return 1;
    if (!strcmp(str[0], "echo"))
        return echo(str, return_value);
    return 3;
}

static int func_cmd(struct ast *ast, int return_value)
{
    for (size_t i = 0; i < ast->data->ast_cmd->arg->size; i++)
        expandinho(&(ast->data->ast_cmd->arg->data[i]));
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
    switch (ast->type)
    {
    case AST_IF:
        return func_if(ast, return_value);
    case AST_LIST:
        return func_list(ast, return_value);
    case AST_CMD:
        return func_cmd(ast, return_value);
    /*case AST_WHILE:
        return func_while(ast, return_value);
    case AST_UNTIL:
        return func_until(ast, return_value);
    case AST_FOR:
        return func_for(ast, return_value);
    case AST_OP:
        return func_operation(ast, return_value);*/
    default:
        return 19;
        // ADD NEW AST EXECUTE HERE
    }
}
