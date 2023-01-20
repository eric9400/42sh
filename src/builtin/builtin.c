#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vector.h"
#include "execute.h"
#include "hash_map.h"
#include "parse_execute_loop.h"

int is_in_dot = 0;

static int is_backslashable(char c)
{
    return c == '\\' || c == '`';
}

static size_t print_special_char(int f_e, char c, int in_d_quotes,
                                 int in_s_quotes)
{
    if (!f_e)
    {
        if (in_s_quotes || (in_d_quotes && !is_backslashable(c)))
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
    int in_s_quotes = 0;
    int in_d_quotes = 0;
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
                j +=
                    print_special_char(f_e, s[j + 1], in_d_quotes, in_s_quotes);
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

int echo(char **s)
{
    if (fcntl(STDOUT_FILENO, F_GETFD))
    {
        fprintf(stderr, "echo: write error: Bad file descriptor\n");
        return 1;
    }
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

int export(char **s)
{
    int lenvar = 0;
    size_t lens = strlen(s[1]);
    int isseton = 0;
    for (size_t i = 0; i < lens; i++, lenvar++)
    {
        if (s[1][i] == '=')
        {
            isseton = 1;
            break;
        }
    }
    if (isseton) //export n=b
        putenv(s[1]); //est ce qu'il faut utiliser putenv
    else
    {
        char *var = strndup(s[1], lenvar);
        const char *value = hash_map_get(hashmap, var);
        if (value != NULL)
        {
            size_t lenvalue = strlen(value); 
            var = realloc(var, lenvar + 1 + lenvalue);
            var[lenvar] = '=';
            size_t j = 0;
            for (size_t i = lenvar + 1; j < lenvalue; i++, j++)
                var[i] = value[j];
            putenv(var);
            free(var);
        }
        else
            free(var);
    }
    return 0;
}

static int is_flag_for_unset(char *s, int *f_f, int *f_v)
{
    if (!s)
        return 1;
    int save_f = *f_f;
    int save_v = *f_v;

    // can be a flag
    if (s[0] == '-')
    {
        size_t len = strlen(s);
        for (size_t i = 1; i < len; i++)
        {
            if (s[i] == 'f')
                *f_f = 1;
            else if (s[i] == 'v')
                *f_v = 1;
            else
            {
                *f_f = save_f;
                *f_v = save_v;
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int unset(char **s) //REMOVE VAR FROM HASH_MAP, ENV_VAR_, AND REMOVE FUNCTIONS
{
    int f_f = 0;
    int f_v = 0;
    int i = 1;
    // parsing
    while (s[i] != NULL)
    {
        // break if detecting a non flag
        if (!is_flag_for_unset(s[i], &f_f, &f_v))
            break;
        i++;
    }

    int return_value = 0;
    while (s[i] != NULL)
    {
        if (f_v || !f_f)
        {
           bool hash_var = hash_map_remove(hashmap, s[i]);
           int env_var = unsetenv(s[i]);
           if (!hash_var && env_var == -1)
               return_value = 1;
        }
        else
        {
            printf("IT HAS TO BE A FUNCTION REMOVE IN HASHMAP\n");
            //if (!hash_map_f_remove(hashmap_f, s[i])
            //  return_value = 1;
        }
        i++;
    }
    return return_value;
}

/*int cd(char **s)
{
   //TO DO 
}*/

int exit_dot(void)
{
    if (file == stdin)
    {
        fprintf(stderr, "Bad file for . builtin\n");
        return 1;
    }
    else
    {
        freeAll(0);
        exit(1);
    }
}

int dot(char **s) //44 lines
{
    char *filename = s[1];
    int has_slash = 0;
    int len_filename = strlen(s[1]);
    for (int i = 0; i < len_filename; i++)
    {
        if (s[1][i] == '/')
        {
            has_slash = 1;
            break;
        }
    }
    FILE *filedot = NULL;
    if ((filedot = fopen(filename, "r")) == NULL)
    {
        if (!has_slash)
        {
            char *path = getenv("PATH");
            char *curr = strtok(path, ":");
            char *all;
            int curlen = strlen(curr);
            while (curr != NULL)
            {
                all = strndup(curr, curlen + 2 + len_filename);
                all[curlen] = '/';
                for (int i = 0; i < len_filename; i++)
                    all[i + curlen + 1] = filename[i];
                if ((filedot = fopen(all, "r")) != NULL)
                {
                    free(all);
                    break;
                }
                free(all);
                strtok(NULL, ":");
            }
            if (!has_slash)
                return exit_dot();
        }
        else
            return exit_dot();
    }
    struct lexer *old_lex = lex;
    lex = NULL;
    struct ast *old_ast = ast;
    ast = NULL;
    FILE *old_file = file;

    int i = 2;
    while (s[i++] != NULL)
    {
        char value[100] = { 0 };
        sprintf(value, "%d", i-1);
        hash_map_insert(hashmap, value, s[i]);
    }
    is_in_dot = 1;
    int res = parse_execute_loop(filedot, global_flags);
    lex = old_lex;
    ast = old_ast;
    file = old_file;
    is_in_dot = 0;
    return res;
}

int my_exit(char **s, int return_value)
{
    if (s[1] == NULL)
        exit(return_value);
    int isnum = 1;
    int len = strlen(s[1]);
    for (int i = 0; i < len; i++)
    {
        if (s[1][i] < '0' || s[1][i] > '9')
        {
            isnum = 0;
            break;
        }
    }
    if (!isnum)
    {
        fprintf(stderr, "exit: need numeric argument\n");
        exit(1);
    }
    if (s[2] != NULL)
    {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }
    exit(atoi(s[1]));
}

int corb(char **s, struct c_or_b *no_to_racismo, int i)
{
    if (s[1] == NULL)
    {
        no_to_racismo->is_break = i;
        no_to_racismo->cbdeep = 1;
        return 1;
    }
    else
    {
        int r = atoi(s[1]);
        if (s[2] != NULL || r == 0)
        {
            fprintf(stderr, "Bad argument\n");
            return 128;
        }
        if(no_to_racismo->is_in_loop)
        {
            no_to_racismo->is_break = i;
            no_to_racismo->cbdeep = r;
        }
        return 0;
    }
}

int check_builtin(char **str, struct c_or_b *no_to_racismo, int return_value)
{
    if (!strcmp(str[0], "true"))
        return 0;
    if (!strcmp(str[0], "false"))
        return 1;
    if (!strcmp(str[0], "echo"))
        return echo(str);
    if (!strcmp(str[0], "export"))
        return export(str);
    if (!strcmp(str[0], "unset"))
        return unset(str);
    if (!strcmp(str[0], "continue"))
        return corb(str, no_to_racismo, 0);
    if (!strcmp(str[0], "break"))
        return corb(str, no_to_racismo, 1);
    if (!strcmp(str[0], "exit"))
        return my_exit(str, return_value);
    if (!strcmp(str[0], "."))
        return dot(str);
    /*if (!strcmp(str[0], "cd"))
        return cd(str);*/
    return -1;
}
