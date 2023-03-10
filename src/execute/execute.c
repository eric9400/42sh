#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast.h"
#include "ast_free.h"
#include "builtin.h"
#include "execute_tools.h"
#include "expand_tools.h"
#include "f_hash_map.h"
#include "fnmatch.h"
#include "hash_map.h"
#include "hash_map_global.h"
#include "lexer.h"
#include "parse_execute_loop.h"
#include "pipe.h"
#include "redirection.h"

static char buf[] = "     ⠀⠀⠀⠀⠀⠀⣠⣴⣶⣿⣿⣷⣶⣄⣀⣀⠀⠀⠀⠀⠀\n\
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
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀";

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
static void swap_vector(struct ast *ast, struct vector **vect_copy);

static struct c_or_b wat = { 0, 0, 0, -1 };

// 21 lines
static int func_while(struct ast *ast, int return_value)
{
    int res = 0;
    wat.is_in_loop = 1;
    wat.loop_deep++;
    while (!execute(ast->data->ast_while->condition, return_value))
    {
        res = execute(ast->data->ast_while->while_body, return_value);
        if (wat.is_in_loop && wat.is_break != -1)
        {
            if (wat.loop_deep == 1 || wat.cbdeep == 1)
            {
                wat.loop_deep = 0;
                wat.cbdeep = 0;
                if (wat.is_break) // break
                {
                    wat.is_break = -1;
                    wat.is_in_loop = 0;
                    break;
                }
                else // continue
                    wat.is_break = -1;
            }
            else
                break;
        }
    }
    if (wat.is_in_loop)
    {
        wat.cbdeep--;
        wat.loop_deep--;
    }
    return res;
}

// 21 lines
static int func_until(struct ast *ast, int return_value)
{
    int res = 0;
    wat.is_in_loop = 1;
    wat.loop_deep++;
    while (execute(ast->data->ast_until->condition, return_value))
    {
        res = execute(ast->data->ast_until->until_body, return_value);
        if (wat.is_in_loop && wat.is_break != -1)
        {
            if (wat.loop_deep == 1 || wat.cbdeep == 1)
            {
                wat.loop_deep = 0;
                wat.cbdeep = 0;
                if (wat.is_break)
                {
                    wat.is_break = -1;
                    wat.is_in_loop = 0;
                    break;
                }
                else
                    wat.is_break = -1;
            }
            else
                break;
        }
    }
    if (wat.is_in_loop)
    {
        wat.cbdeep--;
        wat.loop_deep--;
    }
    return res;
}

// 27 lines
static int func_for(struct ast *ast, int return_value)
{
    int res = 0;
    struct vector *vect_copy = vector_copy(ast->data->ast_for->arg, 1);
    expandinho_phoenix(ast, return_value);
    wat.is_in_loop = 1;
    wat.loop_deep++;
    for (size_t i = 0; i < ast->data->ast_for->arg->size; i++)
    {
        hash_map_insert(hashM->hashmap, ast->data->ast_for->var,
                        ast->data->ast_for->arg->data[i]);
        res = execute(ast->data->ast_for->for_list, return_value);
        if (wat.is_in_loop && wat.is_break != -1)
        {
            if (wat.loop_deep == 1 || wat.cbdeep == 1)
            {
                wat.loop_deep = 0;
                wat.cbdeep = 0;
                if (wat.is_break)
                {
                    wat.is_break = -1;
                    wat.is_in_loop = 0;
                    break;
                }
                else
                    wat.is_break = -1;
            }
            else
                break;
        }
    }
    hash_map_remove(hashM->hashmap, ast->data->ast_for->var);
    swap_vector(ast, &vect_copy);
    if (wat.is_in_loop)
    {
        wat.cbdeep--;
        wat.loop_deep--;
    }
    return res;
}

static int func_and(struct ast *ast, int return_value)
{
    if (wat.is_in_loop && wat.is_break != -1)
        return 0;
    int left = execute(ast->data->ast_and->left, return_value);
    if (left || (wat.is_in_loop && wat.is_break != -1))
        return left;
    return execute(ast->data->ast_and->right, return_value);
}

static int func_or(struct ast *ast, int return_value)
{
    if (wat.is_in_loop && wat.is_break != -1)
        return 0;
    int left = execute(ast->data->ast_or->left, return_value);
    if (!left)
        return 0;
    return execute(ast->data->ast_or->right, return_value);
}

static int func_not(struct ast *ast, int return_value)
{
    if (wat.is_in_loop && wat.is_break != -1)
        return 0;
    return !execute(ast->data->ast_not->node, return_value);
}

static int func_if(struct ast *ast, int return_value)
{
    if (wat.is_in_loop && wat.is_break != -1)
        return 0;
    int a = execute(ast->data->ast_if->condition, return_value);
    if (!a)
        return execute(ast->data->ast_if->then, return_value);
    return execute(ast->data->ast_if->else_body, return_value);
}

static int func_list(struct ast *ast, int return_value)
{
    size_t size = ast->data->ast_list->size - 1;

    for (size_t i = 0; i < size; i++)
    {
        execute(ast->data->ast_list->cmd_if[i], return_value);
        if (wat.is_in_loop && wat.is_break != -1)
            return 0;
    }

    // only check last return code from the command
    return execute(ast->data->ast_list->cmd_if[size], return_value);
}

// 38 lines
struct stock_fd *func_redir(struct ast_list *redir, int return_value,
                            int *error)
{
    struct stock_fd *stock_fd = NULL;
    int res = 0;
    for (size_t i = 0; i < redir->size && !res; i++)
    {
        char *tmp = expandinho_phoenix_junior(
            redir->cmd_if[i]->data->ast_redir->exit_file, return_value);
        free(redir->cmd_if[i]->data->ast_redir->exit_file);
        redir->cmd_if[i]->data->ast_redir->exit_file = tmp;
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

static void swap_vector(struct ast *ast, struct vector **vect_copy)
{
    if (ast->type == AST_CMD)
    {
        vector_destroy(ast->data->ast_cmd->arg);
        ast->data->ast_cmd->arg = *vect_copy;
    }
    else if (ast->type == AST_FOR)
    {
        vector_destroy(ast->data->ast_for->arg);
        ast->data->ast_for->arg = *vect_copy;
    }
}

// 38 lines
static int func_cmd(struct ast *ast, int return_value)
{
    if (wat.is_in_loop && wat.is_break != -1)
        return 0;
    int error_redir = 0;
    struct stock_fd *stock_fd =
        func_redir(ast->data->ast_cmd->redir, return_value, &error_redir);
    if (stock_fd == NULL && error_redir != 0)
        return error_redir;
    struct vector *vect_copy = vector_copy(ast->data->ast_cmd->arg, 0);
    if (expandinho_phoenix(ast, return_value) == 1)
    {
        destroy_stock_fd(stock_fd);
        swap_vector(ast, &vect_copy);
        return 0;
    }

    int code = check_builtin(ast->data->ast_cmd->arg->data, &wat, return_value);
    if (code != -1)
    {
        destroy_stock_fd(stock_fd);
        swap_vector(ast, &vect_copy);
        return code;
    }
    code = check_function(ast->data->ast_cmd->arg->data, return_value);
    if (code != -1)
    {
        destroy_stock_fd(stock_fd);
        vector_destroy(vect_copy);
        return code;
    }
    int pid = fork();

    // child
    if (!pid)
    {
        errno = 0;
        execvp(ast->data->ast_cmd->arg->data[0], ast->data->ast_cmd->arg->data);
        if (errno == ENOENT)
        {
            fprintf(stderr, "%s\n'%s': command not found\n", buf,
                    ast->data->ast_cmd->arg->data[0]);
            exit(127);
        }
        if (errno == ENOEXEC)
            exit(126);

        exit(2);
    }
    destroy_stock_fd(stock_fd);
    int status;
    waitpid(pid, &status, 0);
    swap_vector(ast, &vect_copy);
    return WEXITSTATUS(status);
}

static int func_sub(struct ast *ast, int return_value)
{
    pid_t pid = fork();

    if (!pid)
    {
        int err = execute(ast->data->ast_subshell->sub, return_value);
        exit(err);
    }
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
    // return 0;
}

static void add_func(struct ast *old)
{
    struct ast *ast = malloc(sizeof(struct ast));
    ast->type = AST_FUNC;
    ast->data = malloc(sizeof(union ast_union));
    struct ast_func *ast_func = calloc(1, sizeof(struct ast_func));
    ast_func->func = old->data->ast_func->func;
    ast_func->name = old->data->ast_func->name;
    ast_func->redir = old->data->ast_func->redir;
    old->data->ast_func->func = NULL;
    old->data->ast_func->name = NULL;
    old->data->ast_func->redir = NULL;
    ast->data->ast_func = ast_func;
    f_hash_map_insert(hashM->fhashmap, ast->data->ast_func->name, ast);
}

static int func_case_items(char *cond, struct ast *ast, int return_value)
{
    int is_correct = 0;
    for (size_t i = 0; i < ast->data->ast_case_item->patterns->size; i++)
    {
        char *temp = expandinho_phoenix_junior(
            ast->data->ast_case_item->patterns->data[i], return_value);
        if (!fnmatch(temp, cond, 0))
        {
            free(temp);
            is_correct = 1;
            break;
        }
        free(temp);
    }

    if (is_correct)
        execute(ast->data->ast_case_item->body, return_value);
    return is_correct;
}

static int func_case(struct ast *ast, int return_value)
{
    char *cond = ast->data->ast_case->value;

    cond = expandinho_phoenix_junior(cond, return_value);

    for (size_t i = 0; i < ast->data->ast_case->items->size; i++)
    {
        if (func_case_items(cond, ast->data->ast_case->items->cmd_if[i],
                            return_value))
            break;
    }

    free(cond);
    return 0;
}

/*
 * 32 lines
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
    case AST_PIPE:
        return func_pipe(ast, return_value);
    case AST_FUNC:
        add_func(ast);
        return 0;
    case AST_SUBSHELL:
        return func_sub(ast, return_value);
    case AST_CASE:
        return func_case(ast, return_value);
    default:
        return 19;
        // ADD NEW AST EXECUTE HERE
    }
}
