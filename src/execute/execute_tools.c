#include "execute_tools.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "hash_map.h"

void split_vector(int *marker, struct ast *ast)
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
    v = vector_append(v, NULL);
    vector_destroy(ast->data->ast_cmd->arg);
    ast->data->ast_cmd->arg = v;
}

int is_char_variable(char c)
{
    return  (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// return "$@"
void split_quote_at(struct vector **v)
{
    int count = 1;
    while (1)
    {
        char key[1000];
        sprintf(key, "%d", count);
        const char *value = hash_map_get(hashmap, key);
        if (!value)
            return;
        *v = vector_append(*v, strdup(value));
        count++;
    }
}

void split_and_append(struct vector **v, char *s)
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

// return $* or $@
void split_no_quote(struct vector **v)
{
    int count = 1;
    while (1)
    {
        char key[1000];
        sprintf(key, "%d", count);
        const char *value = hash_map_get(hashmap, key);
        if (!value)
            return;
        split_and_append(v, strdup(value));
        count++;
    }
}

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

// return "$*"
void split_quote_star(struct vector **v)
{
    int count = 1;
    char *str = malloc(1);
    size_t len_str = 1;
    while (1)
    {
        if (len_str != 1)
            str = strcat(str, " ");
        char key[1000];
        sprintf(key, "%d", count);
        const char *value = hash_map_get(hashmap, key);
        if (!value)
            return;
        len_str += strlen(value) + 1;
        str = realloc(str, sizeof(char) * len_str);
        str = strcat(str, strdup(value));
        count++;
    }
    str[len_str - 1] = '\0';
    *v = vector_append(*v, str);
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

/*
 * S'occupe de append dans v les mots un a un selon les cas
static void expandinho_senior_aux(struct vector **v, char *s)
{
    int in_double_quotes = 0;
    int in_single_quotes = 0;
    size_t len = strlen(s);
    size_t i = 0;
    for (size_t j = 0; j < len; j++)
    {
        if (s[j] == '\'')
            in_single_quotes = !in_single_quotes;
        else if (s[j] == '"')
            in_double_quotes = !in_double_quotes;
        else if (s[j] == '$')
        {
            if (s[j + 1] != '\0' && !in_single_quotes)
            {
                j++;
                if (s[j] == '@')
                {
                    // $@
                    if (!in_double_quotes)
                        split_no_quote(v);
                    // "$@"
                    else
                        split_quote_at(v);
                }
                else if (s[j] == '*')
                {
                    // $*
                    if (!in_double_quotes)
                        split_no_quote(v);
                    // "$*" ok
                    else
                        split_quote_star(v);
                }
            }
        }
        else
        {

        }
    }

}

 * Preparsing for the cases $@ and $*
static void expandinho_senior(struct ast *ast)
{
    struct vector **v = &ast->data->ast_cmd->arg;
    size_t len_vector = ast->data->ast_cmd->arg->size - 1;
    struct vector *new = vector_init(len_vector);
    char **data = ast->data->ast_cmd->arg->data;
    size_t len = 0;
    for (size_t i = 0; i < len_vector; i++)
    {
        char *temp = strdup(data[i]);
        expandinho_senior_aux(v, temp);
        free(temp);
    }
    new = vector_append(new, NULL);
    vector_destroy(*v);
    *v = new;
}
*/

static void expandinho_junior(size_t *i, int *lenvar, int *indnew, char **value)
{
    *i += *lenvar - 1;
    (*indnew)--;
    free(*value);
    *value = NULL;
}

void expandinho(char **str, int return_value, int *marker,
        size_t ind_marker)
{
    if (!(*str))
        return;

    // new string after expand
    size_t len = strlen(*str);
    char *new = malloc(len + 1);
    int indnew = 0;

    int new_len = len;
    int single_quote = 0;

    // temporary variable
    int lenvar = 0;
    size_t value_len = 0;
    char *hkey = NULL;
    char *value = NULL;

    for (size_t i = 0; i < len; i++, indnew++)
    {
        if ((*str)[i] == '$' && !single_quote)
        {
            // we found matching expand
            if (is_still_variable(*str + i + 1, &lenvar) == 0)
            {
                // lenvar == strlen(name var to expand) + '$'
                hkey = strndup(*str + i + 1, lenvar - 1);
                if ((value = is_special_var(hkey, return_value)) == NULL)
                    value = hashmap_get_copy(hashmap, hkey);
                free(hkey);
            }
            // no matching expand
            if (!value)
            {
                if (new_len - lenvar > 0)
                    new = realloc(new, new_len - lenvar + 1);
                else
                    new = realloc(new, 1);
            }
            else
            {
                value_len = strlen(value);
                new_len = new_len - lenvar + value_len;
                new = realloc(new, new_len + 1);
                for (size_t j = 0; j < value_len; j++, indnew++)
                    new[indnew] = value[j];
                marker[ind_marker]++;
            }
            expandinho_junior(&i, &lenvar, &indnew, &value);
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
