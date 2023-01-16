#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "ast.h"
#include "builtin.h"
#include "execute_tools.h"
#include "hash_map.h"
#include "redirection.h"

static char buf[] = 
"     ⠀⠀⠀⠀⠀⠀⣠⣴⣶⣿⣿⣷⣶⣄⣀⣀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣾⣿⣿⡿⢿⣿⣿⣿⣿⣿⣿⣿⣷⣦⡀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⡟⠁⣰⣿⣿⣿⡿⠿⠻⠿⣿⣿⣿⣿⣧\n\
⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⠏⠀⣴⣿⣿⣿⠉⠀⠀⠀⠀⠀⠈⢻⣿⣿⣇\n\
⠀⠀⠀⠀⢀⣠⣼⣿⣿⡏⠀⢠⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⣿⡀\n\
⠀⠀⠀⣰⣿⣿⣿⣿⣿⡇⠀⢸⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⡇\n\
⠀⠀⢰⣿⣿⡿⣿⣿⣿⡇⠀⠘⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⢀⣸⣿⣿⣿⠁\n\
⠀⠀⣿⣿⣿⠁⣿⣿⣿⡇⠀⠀⠻⣿⣿⣿⣷⣶⣶⣶⣶⣶⣿⣿⣿⣿⠃\n\
⠀⢰⣿⣿⡇⠀⣿⣿⣿⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁\n\
⠀⢸⣿⣿⡇⠀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠉⠛⠛⠛⠉⢉⣿⣿\n\
⠀⢸⣿⣿⣇⠀⣿⣿⣿⠀⠀⠀⠀⠀⢀⣤⣤⣤⡀⠀⠀⢸⣿⣿⣿⣷⣦\n\
⠀⠀⢻⣿⣿⣶⣿⣿⣿⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⣦⡀⠀⠉⠉⠻⣿⣿⡇\n\
⠀⠀⠀⠛⠿⣿⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠈⠹⣿⣿⣇⣀⠀⣠⣾⣿⣿⡇\n\
⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⣦⣤⣤⣤⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⡟\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠻⢿⣿⣿⣿⣿⣿⣿⠿⠋⠉⠛⠋⠉⠉⠁\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀"
;

int execute(struct ast *ast, int return_value);
static int func_if(struct ast *ast, int return_value);
static int func_list(struct ast *ast, int return_value);
static int func_cmd(struct ast *ast, int return_value);
static int func_while(struct ast *ast, int return_value);
static int func_until(struct ast *ast, int return_value);
static int func_for(struct ast *ast, int return_value);
static int func_and(struct ast *ast, int return_value);
static int func_or(struct ast *ast, int return_value);
static int func_not(struct ast *ast, int return_value);

//static int func_redir(struct ast *ast, int return_value);
//static int func_pipe(struct ast *ast, int return_value);

static int func_while(struct ast *ast, int return_value)
{
    int res = 0;
    while (!execute(ast->data->ast_while->condition, return_value))
        res = execute(ast->data->ast_while->while_body, return_value);
    return res;    
}

static int func_until(struct ast *ast, int return_value)
{
    int res = 0;
    while (execute(ast->data->ast_until->condition, return_value))
        res = execute(ast->data->ast_until->until_body, return_value);
    return res;
}

static int func_for(struct ast *ast, int return_value)
{
    int res = 0;
    for (size_t i = 0; i < ast->data->ast_for->arg->size; i++)
    {
        hash_map_insert(hashmap, ast->data->ast_for->var, ast->data->ast_for->arg->data[i]); 
        res = execute(ast->data->ast_for->for_list, return_value);
    }
    return res;
}

static int func_and(struct ast *ast, int return_value)
{
    int left = execute(ast->data->ast_and->left, return_value);
    if (left)
        return left;
    return execute(ast->data->ast_and->right, return_value);
}

static int func_or(struct ast *ast, int return_value)
{
    int left = execute(ast->data->ast_or->left, return_value);
    if (!left)
        return 0;
    return execute(ast->data->ast_or->right, return_value);
}

static int func_not(struct ast *ast, int return_value)
{
    return !execute(ast->data->ast_not->node, return_value);
}

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
    return execute(ast->data->ast_list->cmd_if[size], return_value);
}

static struct stock_fd *func_redir(struct ast_list *redir, int return_value, int *error)
{
    struct stock_fd *stock_fd = NULL;
    int res = 0;
    for (size_t i = 0; i < redir->size && !res; i++)
    {
        int *marker = calloc(2, sizeof(int));
        expandinho(&(redir->cmd_if[i]->data->ast_redir->exit_file), return_value, marker, 0);
        free(marker);
        switch (redir->cmd_if[i]->data->ast_redir->type)
        {
            case S_RIGHT:
                res = redir_s_right(redir->cmd_if[i], &stock_fd);
                break;
            case S_LEFT:
                res = redir_s_left(redir->cmd_if[i], &stock_fd, 0);
                break;
            case D_RIGHT:
                res = redir_d_right(redir->cmd_if[i], &stock_fd);
                break;
            case RIGHT_AND:
                res = redir_right_and(redir->cmd_if[i], &stock_fd);
                break;
            case LEFT_AND:
                res = redir_left_and(redir->cmd_if[i], &stock_fd);
                break;
            case RIGHT_PIP:
                res = redir_right_pip(redir->cmd_if[i], &stock_fd);
                break;
            case LEFT_RIGHT:
                res = redir_left_right(redir->cmd_if[i], &stock_fd);
                break;
            default:
                // check error
                fprintf(stderr, "C LA MERDEEEEEEEEEEE");
                return stock_fd;
        }
    }
    if (res)
    {
        fprintf(stderr, "redirection: something went wrong\n");
        destroy_stock_fd(stock_fd);
        *error = 1;
        return NULL;
    }
    return stock_fd;
}

static int func_cmd(struct ast *ast, int return_value)
{
    int error_redir = 0;
    struct stock_fd *stock_fd = func_redir(ast->data->ast_cmd->redir, return_value, &error_redir);
    if (stock_fd == NULL && error_redir != 0)
        return error_redir;
    size_t size = ast->data->ast_cmd->arg->size;
    int *marker = calloc(size, sizeof(int));
    //CALL NEW FUNCTION TO EXPAND $@ OR $* AND REARRANGE VECTOR
    // expandinho_senior(ast);

    // check for expand
    for (size_t i = 0; i < size; i++)
        expandinho(&(ast->data->ast_cmd->arg->data[i]), return_value, marker,
                i);

    // split when expanded
    split_vector(marker, ast);
    free(marker);

    int code = check_builtin(ast->data->ast_cmd->arg->data, return_value);
    if (code != -1)
        return code;
    int pid = fork();

    // child
    if (!pid)
    {
        execvp(ast->data->ast_cmd->arg->data[0], ast->data->ast_cmd->arg->data);
        if (errno == ENOENT)
        {
            fprintf(stderr, "%s\n", buf);
            fprintf(stderr, "%s: command not found\n", ast->data->ast_cmd->arg->data[0]);
            exit(127);
        }
        if (errno == ENOEXEC)
            exit(126);
        exit(2);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    destroy_stock_fd(stock_fd);
    return 0;
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
        case AST_WHILE:
            return func_while(ast, return_value);
        case AST_UNTIL:
            return func_until(ast, return_value);
        case AST_FOR:
            return func_for(ast, return_value);
        case AST_AND:
            return func_and(ast, return_value);
        case AST_OR:
            return func_or(ast, return_value);
        case AST_NOT:
            return func_not(ast, return_value);
        /*case AST_PIPE:
            return func_pipe(ast, return_value);
        */default:
            return 19;
            // ADD NEW AST EXECUTE HERE
    }
}
