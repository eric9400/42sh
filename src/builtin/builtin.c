#include <stdio.h>
#include <string.h>

#include "vector.h"

static size_t print_special_char(int f_e, char c, int in_d_quotes, int in_s_quotes)
{
    if (!f_e)
    {
        if (in_d_quotes || in_s_quotes)
            printf("\\");
        printf("%c", c);
        return 1;
    }

    size_t res = 1;
    if (c == 'n')
        printf("\n");
    else if (c == 't')
        printf("\t");
    // factorize case c is and is not '\\'
    else
    {
        printf("\\");
        // case '\' with non escape next
        if (c != '\\')
            res = 0;
    }
    return res;
}

static void print_echo(int f_n, int f_e, struct vector *v)
{
    int in_s_quotes= 0;
    int in_d_quotes= 0;
    for (size_t i = 0; i < v->size; i++)
    {
        char *s = v->data[i];
        for (size_t j = 0; j < strlen(s); j++)
        {
            if (!in_d_quotes && s[j] == '\'')
            {
                in_s_quotes = !in_s_quotes;
                continue;
            }
            else if (!in_s_quotes && s[j] == '"')
            {
                in_d_quotes = !in_d_quotes;
                continue;
            }
            // we have to expand everything from here
            else if (s[j] == '\\')
                j += print_special_char(f_e, s[j + 1], in_d_quotes, in_s_quotes);
            else
                printf("%c", s[j]);
        }
        in_s_quotes = 0;
        in_d_quotes = 0;
        if (i != v->size - 1)
            printf(" ");
    }
    if (!f_n)
        printf("\n");
}

static int is_flag(char *s, int *f_n, int *f_e)
{
    if (!s)
        return 1;
    int save_n = *f_n;
    int save_e = *f_e;

    // can be a flag
    if (s[0] == '-')
    {
        size_t len = strlen(s);
        for (size_t i = 1; i < len; i++)
        {
            if (s[i] == 'n')
                *f_n = 1;
            else if (s[i] == 'e')
                *f_e = 1;
            else if (s[i] == 'E')
                *f_e = 0;
            else
            {
                *f_n = save_n;
                *f_e = save_e;
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int echo(char **s, int return_value)
{
    (void)return_value;
    if (s && strcmp(s[0], "echo"))
        return 1;
    int f_n = 0;
    int f_e = 0;
    struct vector *v = vector_init(10);
    size_t i = 1;

    // parsing
    while (s[i] != NULL)
    {
        // break if detecting a non flag
        if (!is_flag(s[i], &f_n, &f_e))
            break;
        i++;
    }

    // add all strings supposed to be printed
    while (s[i] != NULL)
        v = vector_append(v, strdup(s[i++]));

    // both flags set => disable interpretation of \n etc
    print_echo(f_n, f_e, v);

    vector_destroy(v);
    fflush(stdout);
    return 0;
}
