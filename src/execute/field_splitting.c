#include "field_splitting.h"

#include <string.h>
#include <stdlib.h>

#include "hash_map.h"
#include "hash_map_global.h"
#include "vector.h"

static char delim[100] = { 0 };

static int get_delim_ifs()
{
    char *ifs = hash_map_get(hashM->hashmap, "IFS");
    if (!ifs)
        return 0;
    if (ifs[0] == '\0')
    {
        delim[0] = ' ';
        delim[1] = '\n';
        delim[2] = '\t';
        delim[3] = '\0';
    }
    else
    {
        if (strcmp(ifs, delim))
        {
            int i = 0;
            while (ifs[i] != '\0')
            {
                delim[i] = ifs[i];
                i++;
            }
            delim[i] = '\0';
        }
    }
    return 1;
}

static int is_in(char c)
{
    int i = 0;
    while (delim[i] != '\0')
    {
        if (delim[i] == c)
            return 1;
        i++;
    }
    return 0;
}

static int following_is_valid(char c)
{
    return c == '\'' || c == '\\' || c == '`' || c == '$' || c == '"';
}

void field_split(struct vector **v, enum ast_type type)
{
    if (!get_delim_ifs())
        return;
    int ind_para = 0;
    int len = (*v)->size - 1;
    if (type == AST_FOR)
        len++;
    struct vector *new = vector_init((*v)->capacity);
    int in_single_quotes;
    int in_double_quotes;
    int len_str;
    char *curr = NULL;
    char *to_append = NULL;
    int len_to_append;
    for (; ind_para < len; ind_para++)
    {
        curr = (*v)->data[ind_para];
        len_str = strlen(curr);
        to_append = calloc(len_str + 1, 1);
        in_single_quotes = 0;
        in_double_quotes = 0;
        len_to_append = 0;
        for (int i = 0; i < len_str; i++)
        {
            if (!in_single_quotes && curr[i] == '\\')
            {
                if (in_double_quotes)
                {
                    if (following_is_valid(curr[i + 1]))
                        to_append[len_to_append++] = curr[++i];
                    else
                        to_append[len_to_append++] = curr[i];
                }
                else if (curr[i + 1] == '\n')
                    i++;
                else
                    to_append[len_to_append++] = curr[++i];
            }
            else if (!in_double_quotes && curr[i] == '\'')
                in_single_quotes = !in_single_quotes;
            else if (!in_single_quotes && curr[i] == '"')
                in_double_quotes = !in_double_quotes;
            else if (!in_single_quotes && is_in(curr[i]) && to_append[0] != '\0')
            {
                vector_append(new, strdup(to_append));
                free(to_append);
                len_to_append = 0;
                to_append = calloc(len_str + 1, 1);
            }
            else
                to_append[len_to_append++] = curr[i];
        }
        if (to_append[0] != '\0')
            vector_append(new, strdup(to_append));
        else 
            vector_append(new, strdup(""));
        free(to_append);
    }
    vector_destroy(*v);
    if (type == AST_CMD)
        vector_append(new, NULL);
    *v = new;
}
