#define _XOPEN_SOURCE 600

#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vector.h"
#include "execute.h"
#include "hash_map.h"
#include "parse_execute_loop.h"

int is_in_dot = 0;

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

static int echo(char **s)
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

static int export(char **s)
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

static int unset(char **s) //REMOVE VAR FROM HASH_MAP, ENV_VAR_, AND REMOVE FUNCTIONS
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

static int cd(char **s)
{
    int res = 0;
    char cwd[PATH_MAX];
    char cwd2[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if (s[1] == NULL)
    {
        char *home = getenv("HOME");
        if (home == NULL) //STEP 1
        {
            fprintf(stderr, "cd: HOME undefined\n");
            return 1;
        }
        res = chdir(home); //STEP 2
        if (!res)
        {
            hash_map_insert(hashmap, "OLDPWD", cwd);
            getcwd(cwd2, sizeof(cwd2));
            hash_map_insert(hashmap, "PWD", cwd2);
            return 0;
        }
        fprintf(stderr, "cd: error with HOME\n");
        return 1;
    }
    if (s[2] != NULL)
    {
        fprintf(stderr, "cd: too much arguments\n");
        return 1;
    }
    if (!strcmp(s[1], "-"))
    {
        const char *oldpwd = hash_map_get(hashmap, "OLDPWD");
        res = chdir(oldpwd);
        if (!res)
        {
            hash_map_insert(hashmap, "OLDPWD", cwd);
            getcwd(cwd2, sizeof(cwd2));
            hash_map_insert(hashmap, "PWD", cwd2);
            printf("%s\n", cwd2);
            return 0;
        }
    }
    res = chdir(s[1]);
    if (!res)
    {
        hash_map_insert(hashmap, "OLDPWD", cwd);
        getcwd(cwd2, sizeof(cwd2));
        hash_map_insert(hashmap, "PWD", cwd2);
        return 0;
    }
    fprintf(stderr, "cd: wrong directory (might be something else)\n");
    return 1;
}

static int exit_dot(void)
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

static void hash_map_restore(char **values)
{
    int i = 0;
    char *value = NULL;
    char key[4];
    sprintf(key, "%d", i+1);
    while (values[i] != NULL)
    {
        hash_map_insert(hashmap, key, value);
        free(values[i]);
        i++;
        sprintf(key, "%d", i+1);
    }
    while ((value = hash_map_get(hashmap, key)) != NULL)
    {
        hash_map_remove(hashmap, key);
        i++;
        sprintf(key, "%d", i+1);
    }
    free(values);
}

static char **copy_values()
{
    char **result = calloc(100, 1);
    int len = 0;
    char key[4];
    char *value;
    sprintf(key, "%d", len+1);
    while ((value = hash_map_get(hashmap, key)) != NULL)
    {
        result[len] = strdup(value);
        len++;
        sprintf(key, "%d", len+1);
    }
    result = realloc(result, (len+1) * sizeof(char *));
    return result;
}

static int dot2(char **s, FILE *filedot)
{
    struct lexer *old_lex = lex;
    lex = NULL;
    struct ast *old_ast = ast;
    ast = NULL;
    FILE *old_file = file;
    char **old_hashmap = copy_values();
    int i = 2;
    while (s[i++] != NULL)
    {
        char value[4] = { 0 };
        sprintf(value, "%d", i-1);
        hash_map_insert(hashmap, value, s[i]);
    }
    is_in_dot = 1;
    int res = parse_execute_loop(filedot, global_flags);
    hash_map_restore(old_hashmap);
    lex = old_lex;
    ast = old_ast;
    file = old_file;
    is_in_dot = 0;
    return res;
}

static int dot(char **s)
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
    return dot2(s, filedot);
}

static int my_exit(char **s, int return_value)
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

static int corb(char **s, struct c_or_b *no_to_racismo, int i)
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
    if (!(*str))
        return -1;
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
    if (!strcmp(str[0], "cd"))
        return cd(str);
    return -1;
}
