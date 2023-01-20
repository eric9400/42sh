#include "expand_tools.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "hash_map.h"
#include "my_string.h"

static int is_valid_char(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else if (c >= '0' && c <= '9')
        return 1;
    return c == '_';
}

static void resize_string(struct string *new_str, char *buf)
{
    size_t size = new_str->index + strlen(buf) + 1;
    if (size < new_str->len)
        return;
    new_str->len = size * 2;
    new_str->str = realloc(new_str->str, new_str->len);
}

void string_append(struct string *new_str, char *buf)
{
    resize_string(new_str, buf);
    my_strcat(new_str->str + new_str->index, buf);
    new_str->index += strlen(buf);
}

static inline char *is_special_var(char *str, int return_value)
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

static void expand_from_hashmap(struct string *new_str, char *buf, int return_value)
{
    char *value = NULL;
    if ((value = is_special_var(buf, return_value)) == NULL)
        value = hashmap_get_copy(hashmap, buf);
    if (value)
    {
        string_append(new_str, value);
        free(value);
    }
}

static int is_special_char(char c)
{
    return c == '@' || c == '?' || c == '$' || c == '#' || c == '*';
}

int dollar_expansion(struct string *str, struct string *new_str, int return_value)
{
    str->index += 1;
    char buf[5] = { 0 };
    // ${a}
    // need to handle error cases "echo ${+}" for example
    if (str->str[str->index] == '{')
    {
        size_t start = str->index;
        while (is_valid_char(str->str[str->index]))
            str->index += 1;
        char *key = strndup(str->str, str->index - start);
        if (str->str[str->index] == '}')
        {
            expand_from_hashmap(new_str, key, return_value);
            free(key);
            return 0;
        }
        fprintf(stderr, "bad substitution %s\n", key);
        free(key);
        // if no matching '}' => error
        return 1;
    }
    // $a $RANDOM $UID $HOME
    else if (is_valid_char(str->str[str->index]))
    {
        // case $n
        if (isdigit(str->str[str->index]))
        {
            buf[0] = str->str[str->index];
            expand_from_hashmap(new_str, buf, return_value);
        }
        // case $abc
        else
        {
            size_t start = str->index;
            while(is_valid_char(str->str[str->index]))
                str->index += 1;
            char *key = strndup(str->str + start, str->index - start);
            expand_from_hashmap(new_str, key, return_value);
            free(key);
        }
    }
    // case $@ $* $? $$ $#
    else if (is_special_char(str->str[str->index]))
    {
        char *key = strndup(str->str + str->index, 1);
        expand_from_hashmap(new_str, key, return_value);
        free(key);
    }
    // non substituable var
    else
    {
        // $_ with invalid char after $, just print $_
        buf[0] = '$';
        buf[1] = str->str[str->index];
        string_append(new_str, buf);
    }
    return 0;
}

static int is_valid_escape_d_quotes(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

void slash_expansion_in_d_quotes(struct string *str, struct string *new_str, int in_d_quotes)
{
    str->index++;
    char buf[3] = { 0 };
    if (is_valid_escape_d_quotes(str->str[str->index]))
    {
        if (str->str[str->index] == '\n')
            return;
        buf[0] = str->str[str->index];
    }
    else
    {
        if (in_d_quotes)
        {
            buf[0] = '\\';
            buf[1] = str->str[str->index];
        }
        else
            buf[0] = str->str[str->index];
    }
    string_append(new_str, buf);
}
