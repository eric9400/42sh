#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "vector.h"

static size_t print_special_char(char c)
{
    if (c == 'n')
        printf("\n");
    else if (c == 't')
        printf("\t");
    // factorize case c is and is not '\\'
    else
    {
        printf("\\");
        // case '\' with non escape next
        if (c != '\\' && c != '\0')
            printf("%c", c);
    }
    return 1;
}

static void print_echo(int f_n, int f_e, struct vector *v)
{
    for (size_t i = 0; i < v->size; i++)
    {
        char *s = v->data[i];
        for (size_t j = 0; j < strlen(s); j++)
        {
            if (f_e && s[j] == '\\')
                j += print_special_char(s[j + 1]);
            else
                printf("%c", s[j]);
        }
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
    if (fcntl(STDOUT_FILENO, F_GETFD))
    {
        fprintf(stderr, "echo: write error: Bad file descriptor\n");
        return 1;
    }
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

int check_builtin(char **str, int return_value)
{
    if (!(*str))
        return -1;
    if (!strcmp(str[0], "true"))
        return 0;
    if (!strcmp(str[0], "false"))
        return 1;
    if (!strcmp(str[0], "echo"))
        return echo(str, return_value);
    return -1;
}
