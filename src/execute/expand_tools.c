#include "expand_tools.h"

#include <stddef.h>
#include <stdlib.h>

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

static void resize_string(char **str, size_t size, size_t *capacity)
{
    if (size < *capacity)
        return;
    *capacity = size * 2;
    *str = realloc(*str, *capacity);
}

void string_append(struct string *str, struct string *new_str, char *buf)
{
    resize_string(&new_str->str, new_str->index + strlen(buf) + 1, new_str->len);
    new_str->str = strcat(new_str->str, buf);
    new_str->index += strlen(buf);
}

static void expand_from_hashmap(struct string *str, struct string *new_str, char *buf)
{
    char *value = hashmap_get_copy(hashmap, buf);
    if (value)
    {
        string_append(str, new_str, value);
        free(value);
    }
}

int dollar_expansion(struct string *str, struct string *new_str)
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
        char *key = strndup(str->str[start], str->index - start);
        if (str->str[str->index] == '}')
        {
            expand_from_hashmap(str, new_str, key);
            free(key);
            return 0;
        }
        fprintf(stderr, "bad substitution %s\n", key);
        free(key);
        // if no matching '}' => error
        return 1;
    }
    // $a
    else if (is_valid_char(str->str[str->index]))
    {
        if (isdigit(str->str[str->index]))
        {
            buf[0] = str->str[str->index];
            expand_from_hashmap(str, new_str, buf);
        }
        else
        {
            size_t start = str->index;
            while(is_valid_char(str->str[str->index]))
                str->index += 1;
            char *key = strndup(str->str[start], str->index - start);
            expand_from_hashmap(str, new_str, key);
            free(key);
        }
    }
    else
    {
        // $_ with invalid char after $, just print $_
        buf[0] = '$';
        buf[1] = str->str[str->index];
        string_append(str, new_str, buf);
    }
    return 0;
}

static int is_valid_escape_d_quotes(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

void slash_expansion_in_d_quotes(struct string *str, struct string *new_str)
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
        buf[0] = '\\';
        buf[1] = str->str[str->index];
    }
    string_append(str, new_str, buf);
}
