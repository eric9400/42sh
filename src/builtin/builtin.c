#define _XOPEN_SOURCE 600

#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "execute.h"
#include "hash_map.h"
#include "hash_map_global.h"
#include "parse_execute_loop.h"
#include "vector.h"

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

// 19 lines
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

// 19 lines
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

void export_insert(char *s)
{
    int i = 0;
    while (tofree->env_variables[i] != NULL)
        i++;
    char *result = strdup(s);
    tofree->env_variables[i] = result;
    putenv(result);
}

// 40 lines
static int export(char **s)
{
    int lenvar = 0;
    size_t lens;
    int isseton = 0;
    int k = 1;
    int returnvalue = 0;
    while (s[k] != NULL)
    {
        isseton = 0;
        lenvar = 0;
        lens = strlen(s[k]);
        for (size_t i = 0; i < lens; i++, lenvar++)
        {
            if (s[k][i] == '=')
            {
                if (i == 0)
                {
                    fprintf(stderr, "export: bad identifier\n");
                    returnvalue = 2;
                }
                isseton = 1;
                break;
            }
        }
        if (returnvalue == 2)
        {
            returnvalue = 1;
            k++;
            continue;
        }
        if (isseton) // export n=b
            export_insert(s[k]);
        else
        {
            char *var = strndup(s[k], lenvar);
            const char *value = hash_map_get(hashM->hashmap, var);
            if (value != NULL)
            {
                size_t lenvalue = strlen(value);
                var = realloc(var, lenvar + 1 + lenvalue + 1);
                var[lenvar] = '=';
                size_t j = 0;
                for (size_t i = lenvar + 1; j < lenvalue; i++, j++)
                    var[i] = value[j];
                var[lenvar + lenvalue + 1] = '\0';
                export_insert(var);
                free(var);
            }
            else
            {
                returnvalue = 1;
                free(var);
            }
        }
        k++;
    }
    return returnvalue;
}

// 17 lines
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

void check_free_unset(char *s)
{
    int i = 0;
    while (i < 10)
    {
        if (tofree->env_variables[i] != NULL
            && !strcmp(tofree->env_variables[i], s))
        {
            free(tofree->env_variables[i]);
            tofree->env_variables[i] = NULL;
            break;
        }
        i++;
    }
}

// 20 lines
// REMOVE VAR FROM HASH_MAP, ENV_VAR_, AND REMOVE FUNCTIONS
static int unset(char **s)
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
            bool hash_var = hash_map_remove(hashM->hashmap, s[i]);
            int env_var = unsetenv(s[i]);
            check_free_unset(s[i]);
            if (!hash_var && env_var == -1)
                return_value = 1;
        }
        else
        {
            if (!f_hash_map_remove(hashM->fhashmap, s[i]))
                return_value = 1;
        }
        i++;
    }
    return return_value;
}

static int quit(void)
{
    fprintf(stderr, "cd: error quit\n");
    return 1;
}

static int symilinki(char *s)
{
    int res = 0;
    char *sym = NULL;
    if (strcmp(s, ".") && strcmp(s, ".."))
    {
        sym = realpath(s, sym);
        if (sym)
        {
            res = chdir(sym);
            free(sym);
        }
        else
            res = chdir(s);
    }
    else
        res = chdir(s);
    return res;
}

// 35 lines
static int cd(char **s)
{
    int res = 0;
    char cwd[PATH_MAX];
    char cwd2[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    if (s[1] == NULL)
    {
        char *home = getenv("HOME");
        if (home == NULL) // STEP 1
            return quit();
        res = chdir(home); // STEP 2
        if (!res)
        {
            hash_map_insert(hashM->hashmap, "OLDPWD", cwd);
            getcwd(cwd2, sizeof(cwd2));
            hash_map_insert(hashM->hashmap, "PWD", cwd2);
            return 0;
        }
        return quit();
    }
    if (s[2] != NULL)
        return quit();
    if (!strcmp(s[1], "-"))
    {
        const char *oldpwd = hash_map_get(hashM->hashmap, "OLDPWD");
        res = chdir(oldpwd);
        if (!res)
        {
            hash_map_insert(hashM->hashmap, "OLDPWD", cwd);
            getcwd(cwd2, sizeof(cwd2));
            hash_map_insert(hashM->hashmap, "PWD", cwd2);
            printf("%s\n", cwd2);
            return 0;
        }
        return quit();
    }
    res = symilinki(s[1]);
    if (!res)
    {
        hash_map_insert(hashM->hashmap, "OLDPWD", cwd);
        getcwd(cwd2, sizeof(cwd2));
        hash_map_insert(hashM->hashmap, "PWD", cwd2);
        return 0;
    }
    return quit();
}

static int exit_dot(void)
{
    if (tofree->file == stdin)
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

// 14 lines
void hash_map_restore(char **values)
{
    int i = 0;
    char *value = NULL;
    char key[1000];
    sprintf(key, "%d", i + 1);
    while (values[i] != NULL)
    {
        hash_map_insert(hashM->hashmap, key, values[i]);
        free(values[i]);
        i++;
        sprintf(key, "%d", i + 1);
    }
    while ((value = hash_map_get(hashM->hashmap, key)) != NULL)
    {
        hash_map_remove(hashM->hashmap, key);
        i++;
        sprintf(key, "%d", i + 1);
    }
    free(values);
}

// 11 lines
char **copy_values(void)
{
    char **result = calloc(100, 1);
    int len = 0;
    char key[1000];
    char *value;
    sprintf(key, "%d", len + 1);
    while ((value = hash_map_get(hashM->hashmap, key)) != NULL)
    {
        result[len] = strdup(value);
        len++;
        sprintf(key, "%d", len + 1);
    }
    result = realloc(result, (len + 1) * sizeof(char *));
    return result;
}

// 24 lines
static int dot2(char **s, FILE *filedot)
{
    struct lexer *old_lex = tofree->lex;
    tofree->lex = NULL;
    struct ast *old_ast = tofree->ast;
    tofree->ast = NULL;
    FILE *old_file = tofree->file;
    char **old_hashmap = copy_values();
    int i = 2;
    char value[1000] = { 0 };
    while (s[i] != NULL)
    {
        sprintf(value, "%d", i - 1);
        hash_map_insert(hashM->hashmap, value, s[i]);
        i++;
    }
    sprintf(value, "%d", i - 1);
    while (hash_map_remove(hashM->hashmap, value))
    {
        i += 1;
        sprintf(value, "%d", i - 1);
    }
    is_in_dot = 1;
    int res = parse_execute_loop(filedot, tofree->global_flags);
    hash_map_restore(old_hashmap);
    tofree->lex = old_lex;
    tofree->ast = old_ast;
    tofree->file = old_file;
    is_in_dot = 0;
    return res;
}

// 29 lines
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

// 17 lines
static int my_exit(char **s, int return_value)
{
    if (s[1] == NULL)
    {
        exit(return_value);
    }
    if (s[2] != NULL)
    {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }
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
        char *testing = hash_map_get(hashM->hashmap, s[1] + 1);
        if (testing == NULL)
        {
            fprintf(stderr, "exit: need numeric argument\n");
            exit(1);
        }
    }
    exit(atoi(s[1]));
}

// 13 lines
static int corb(char **s, struct c_or_b *no_to_racismo, int i)
{
    if (s[1] == NULL)
    {
        no_to_racismo->is_break = i;
        no_to_racismo->cbdeep = 1;
        return 0;
    }
    else
    {
        int r = atoi(s[1]);
        if (s[2] != NULL || r == 0)
        {
            fprintf(stderr, "Bad argument\n");
            return 128;
        }
        if (no_to_racismo->is_in_loop)
        {
            no_to_racismo->is_break = i;
            no_to_racismo->cbdeep = r;
        }
        return 0;
    }
}

static int alias(char **s)
{
    char *str = s[1];
    if (!str)
        return 0;

    struct vector *v = vector_init(10);

    char *key = strdup(str);

    char *value = strstr(key, "=");
    if (!value)
    {
        vector_destroy(v);
        free(key);
        return 1;
    }
    value[0] = '\0';
    value++;
    value = strdup(value);

    char *temp = NULL;
    while ((temp = hash_map_get(hashM->hashmap_alias, value)))
    {
        size_t i = 0;
        // parcourir le vecteur v et checker si on a deja croise la string temp
        for (i = 0; i < v->size; i++)
        {
            if (!strcmp(v->data[i], temp))
                break;
            // si on l'a pas deja croise on append
            free(value);
            value = strdup(temp);
            vector_append(v, temp);
        }
        if (i != v->size)
            break;
    }

    hash_map_insert(hashM->hashmap_alias, key, value);
    vector_destroy(v);
    free(key);
    free(value);
    return 0;
}

static int unalias(char **str)
{
    int err = 0;
    for (size_t i = 0; str[i]; i++)
    {
        if (!hash_map_remove(hashM->hashmap_alias, str[i]))
        {
            err = 1;
            fprintf(stderr, "UNALIAS: %s not found\n", str[i]);
        }
    }
    return err;
}

// 23 lines
int check_builtin(char **str, struct c_or_b *no_to_racismo, int return_value)
{
    if (!(*str))
        return -1;
    else if (!strcmp(str[0], "true"))
        return 0;
    else if (!strcmp(str[0], "false"))
        return 1;
    else if (!strcmp(str[0], "echo"))
        return echo(str);
    else if (!strcmp(str[0], "export"))
        return export(str);
    else if (!strcmp(str[0], "unset"))
        return unset(str);
    else if (!strcmp(str[0], "continue"))
        return corb(str, no_to_racismo, 0);
    else if (!strcmp(str[0], "break"))
        return corb(str, no_to_racismo, 1);
    else if (!strcmp(str[0], "exit"))
        return my_exit(str, return_value);
    else if (!strcmp(str[0], "."))
        return dot(str);
    else if (!strcmp(str[0], "cd"))
        return cd(str);
    else if (!strcmp(str[0], "alias"))
        return alias(str);
    else if (!strcmp(str[0], "unalias"))
        return unalias(str);
    return -1;
}