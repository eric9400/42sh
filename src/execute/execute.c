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


int is_still_variable(char *str, int *lenvar)
{
    int i = 0;
    while (str[i] != '\0' && !isspace(str[i]))
    {
        if ((str[i] <= 'z' && str[i] >= 'a') || (str[i] <= 'Z' && str[i] >= 'A') || (str[i] <= '9' && str[i] >= '0') || str[i] == '_')
        {
            i++;
            continue;
        }
        else if (i == 0 && ((*str == '@') || (*str == '?') || (*str == '$') || (*str == '#') || (*str == '*')) && *(str + 1) == '\0')
        {
            *lenvar = 2;
            return 5;
        }
        else
            return 0;
    }
    *lenvar = i + 1;
    return 1;
}

static char *is_special_var(char *str, int return_value)
{
    char buf[1000];
    if (!strcmp(str, "@") || !strcmp(str, "*") || atoi(str) != 0 || !strcmp(str, "#") || !strcmp(str, "OLDPWD") || !strcmp(str, "PWD") || !strcmp(str, "IFS"))
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

static char *hashmap_get_copy(struct hash_map *hashmap, char *hkey)
{
    const char *res = hash_map_get(hashmap, hkey);
    if (res == NULL)
        return NULL;
    return strdup(res);
}

static void expandinho(char **str, int return_value, int *marker, size_t ind_marker)
{
    if (!(*str))
        return;
    size_t len = strlen(*str);
    char *new = malloc(len + 1);
    int lenvar = 0;
    int indnew = 0;
    int single_quote = 0;
    char *hkey = NULL;
    char *value = NULL;
    int copylen = len;
    int isv = 0;
    for (size_t i = 0; i < len; i++, indnew++)
    {
        if ((*str)[i] == '$' && !single_quote)
        {
            isv = is_still_variable(*str + i + 1, &lenvar);
            if (isv == 5 || isv == 1)
            {
                hkey = strndup(*str + i + 1, lenvar - 1);
                if ((value = is_special_var(hkey, return_value)) == NULL)
                    value = hashmap_get_copy(hashmap, hkey);
            }
            free(hkey);
            if (!value)
            {
                if (copylen - lenvar > 0)
                    new = realloc(new, copylen - lenvar - 1);
                else
                    new = realloc(new, 1);
            }
            else
            {
                copylen = copylen - lenvar + strlen(value);
                new = realloc(new, copylen + 1);
                for (size_t j = 0; j < strlen(value); j++, indnew++)
                    new[indnew] = value[j];
                marker[ind_marker]++;
            }
            i += lenvar - 1;
            indnew--;
            free(value);
            value = NULL;
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

static void split_and_append(struct vector **v, char *s)
{
    size_t i = 0;
    size_t start = 0;
    while (s[i] != '\0')
    {
        if (s[i] == ' ' && i - start > 0)
        {
            char *copy = strndup(s + start, i - start);
            *v = vector_append(*v, copy);
            start = i + 1;
        }
        else if (s[i] == ' ')
            start++;
        i++;
    }
    if (i - start > 0)
        *v = vector_append(*v, strndup(s + start, i - start));
    free(s);
}

static inline void split_vector(int *marker, struct ast *ast)
{
    size_t len = ast->data->ast_cmd->arg->size;
    struct vector *v = vector_init(len + 1);
    char **data = ast->data->ast_cmd->arg->data;
    for (size_t i = 0; i < len - 1; i++)
    {
        // case when we expanded
        if (marker[i])
        {
            char *temp = strdup(data[i]);
            split_and_append(&v, temp);
        }
        else
            v = vector_append(v, strdup(data[i]));
    }
    vector_append(v, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = v;
}

static int func_cmd(struct ast *ast, int return_value)
{
    size_t size = ast->data->ast_cmd->arg->size;
    int *marker = calloc(size, sizeof(int));
    // check for expand
    for (size_t i = 0; i < size; i++)
        expandinho(&(ast->data->ast_cmd->arg->data[i]), return_value, marker, i);

    // split when expanded
    split_vector(marker, ast);
    free(marker);

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
