#include "execute_tools.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "expand_tools.h"
#include "hash_map.h"
#include "my_string.h"

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

static int expandhino_phoenix_destroyer(struct string *str, struct string *new_str, struct vector *v, int return_value)
{
    if (v)
        vector_destroy(v);
    if (str)
        destroy(str);
    if (new_str)
        destroy(new_str);
    return return_value;
}

int expandhino_phoenix(struct ast *ast)
{
    size_t size = ast->data->ast_cmd->arg->size - 1;
    struct vector *vect_temp = vector_init(10);
    int in_s_quotes = 0;
    int in_d_quotes = 0;
    int was_slash = 0;
    size_t start = 0;
    char buf[2] = { 0 };
    struct string *str = NULL;
    struct string *new_str = NULL;
    for (size_t i = 0; i < size; i++)
    {
        str = init_string(ast->data->ast_cmd->arg->data[i], 0, strlen(str));
        new_str = init_string(str->str, 0, strlen(str));
        in_s_quotes = 0;
        in_d_quotes = 0;
        was_slash = 0;
        for (; str->index < str->len; str->index++)
        {
            buf[0] = str->str[str->index];
            // 2.2.2 single quotes
            if (in_s_quotes)
            {
                if (buf[0] == ''')
                    in_s_quotes = 0;
                else
                    string_append(str, new_str, buf);
            }
            // 2.2.3 double quotes
            else if (in_d_quotes)
            {
                if (buf[0] == '$')
                {
                    if (dollar_expansion(str, new_str))
                        // error case
                        return expandhino_phoenix_destroyer(str, new_str, vect_temp, 1);
                }
                else if (buf[0] == '\\')
                    // there is always something after a backslash
                    slash_expansion_in_d_quotes(str, new_str);
                else
                    string_append(&new_str, curr, &capacity, buf);
            }
            // 2.2.1 escape character backslash
            else if (was_slash)
            {
                // skip \'\n'
                if (buf[0] == '\n')
                    continue;
                buf[1] = buf[0];
                buf[0] = '\\';
                string_append(&new_str, curr, &capacity, buf);
            }
            // other char
            else
            {
                in_d_quotes = buf[0] == '"';
                in_s_quotes = buf[0] == ''';
                was_slash = buf[0] == '\\';
                if (!in_d_quotes && !in_s_quotes && !was_slash)
                    string_append(&new_str, curr, &capacity, buf);
            }
        }
        new_str = realloc(new_str, new_curr + 1);
        new_str[new_curr] = '\0';
        vect_temp = vector_append(vect_temp, new_str);
        expandinho_phoenix_destroyer(str, new_str, NULL);
    }
    vect_temp = vector_append(vect_temp, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = vect_temp;
    return 0;
}
