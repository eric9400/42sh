#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "ast.h"
#include "builtin.h"
#include "execute_tools.h"
#include "hash_map.h"

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

/*
 * \return 0 if expand name is valid else return 1
 * lenvar will contain the size of the expand var name + 1 ('$')
 */
static int is_still_variable(char *str, int *lenvar)
{
    // detecting special var
    if (str[0] != '\0'
        && ((str[0] == '@') || (str[0] == '?') || (str[0] == '$')
            || (str[0] == '#') || (str[0] == '*'))
        && str[1] == '\0')
    {
        *lenvar = 2;
        return 0;
    }

    int i = 0;
    int res = 0;
    while (str[i] != '\0' && !isspace(str[i]) && str[i] != '$')
    {
        if (is_char_variable(str[i]))
            i++;
        else
        {
            res = 1;
            break;
        }
    }
    // return when no problem
    *lenvar = i + 1;
    return res;
}

static char *is_special_var(char *str, int return_value)
{
    char buf[1000];

    /*if (!strcmp(str, "@") || !strcmp(str, "*")){}else */
    if (atoi(str) != 0 || !strcmp(str, "#") || !strcmp(str, "OLDPWD") || !strcmp(str, "PWD")
        || !strcmp(str, "IFS"))
    {
        const char *res = hash_map_get(hashmap, str);
        if (!res)
            return NULL;
        return strdup(res);
    }
    else if (!strcmp(str, "?"))
    {
        sprintf(buf, "%d", return_value);
        return strdup(buf);
    }
    else if (!strcmp(str, "$"))
    {
        sprintf(buf, "%d", getpid());
        return strdup(buf);
    }
    else if (!strcmp(str, "RANDOM"))
    {
        srand(time(NULL));
        // max random possible
        int temp = rand() % 32768;
        sprintf(buf, "%d", temp);
        return strdup(buf);
    }
    else if (!strcmp(str, "UID"))
    {
        uid_t uid = getuid();
        sprintf(buf, "%d", uid);
        return strdup(buf);
    }
    return NULL;
}


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

static int func_cmd(struct ast *ast, int return_value)
{
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
}

static int func_redir(struct ast_list *redir, int return_value)
{
    struct stock_fd *stock_fd = NULL;

    for (size_t i = 0; i < redir->size; i++)
    {
        int *marker = calloc(2, sizeof(int));
        expandhino(&(ast->exit_file), return_value, marker, 0);
        switch (ast->data->ast_redir->type)
        {
            case S_RIGHT:
                redir_s_right(ast, &stock_fd);
            case S_LEFT:
                redir_s_left(ast, &stock_fd);
            case D_RIGHT:
                redir_d_right(ast, &stock_fd);
            case RIGHT_AND:
                redir_right_and(ast, &stock_fd);
            case LEFT_AND:
                redir_left_and(vast, &stock_fd);
            case RIGHT_PIP:
                redir_right_pip(ast, &stock_fd);
            case LEFT_RIGHT:
                redir_left_right(ast, &stock_fd);
            default:
                // check error
                // TOCHANGE
                sprintf(stderr, "C LA MERDEEEEEEEEEEE");
                return 999;
        }
    }
    restore(&stock_fd);
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
        /*case AST_REDIR:
            return func_redir(ast, return_value);
        case AST_PIPE:
            return func_pipe(ast, return_value);
        */default:
            return 19;
            // ADD NEW AST EXECUTE HERE
    }
}
