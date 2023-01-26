#include "field_splitting.h"

#include <string.h>
#include <stdlib.h>

#include "hash_map.h"
#include "hash_map_global.h"
#include "vector.h"

static char delim[100] = { 0 };

static void get_delim_ifs()
{
    char *ifs = hash_map_get(hashM->hashmap, "IFS");
    if (!ifs || ifs[0] == '\0')
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
}

static int is_splitable(char *str)
{
    int i = 0;
    int j = 0;
    while (str[i] != '\0')
    {
        while (delim[j] != '\0')
        {
            if (delim[j] == str[i])
                return 1;
            j++;
        }
        j = 0;
        i++;
    }
    return 0;
}

static struct vector *ifs_split_vect(struct vector **vect, int ind, int type)
{
    struct vector *new = vector_init((*vect)->capacity);
    for (int i = 0; i < ind; i++) //set in new until str
        new->data[i] = strdup((*vect)->data[i]);
    new->size += ind;

    char *token = strtok((*vect)->data[ind], delim);
    if (token == NULL)
        vector_append(new, strdup(""));
    else
    {
        while (token != NULL)
        {
            vector_append(new, strdup(token));
            token = strtok(NULL, delim);
        }
    }

    int len_vect = (*vect)->size - 1;
    if (type == 1)
        len_vect++;
    for (int i = ind + 1; i < len_vect++; i++)
        vector_append(new, strdup((*vect)->data[i]));
    vector_destroy(*vect);
    if (!type)
        vector_append(new, NULL);
    return new;
}

void field_split(struct vector **v, enum ast_type type)
{
    get_delim_ifs();
    int ind_data = 0;
    int len = (*v)->size - 1;
    if (type == AST_FOR)
        len++;
    for (; ind_data < len; ind_data++)
    {
        if (is_splitable((*v)->data[ind_data]))
        {
            *v = ifs_split_vect(v, ind_data, type == AST_CMD ? 0 : 1);
            len = (*v)->size - 1;
            if (type == AST_FOR)
                len++;
        }
    }
}
