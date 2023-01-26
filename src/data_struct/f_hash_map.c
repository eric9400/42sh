#include "f_hash_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "ast_free.h"
#include "hash.h"

struct f_hash_map *f_hash_map_init(size_t size)
{
    struct f_hash_map *hm = malloc(sizeof(struct f_hash_map));
    if (!hm)
        return NULL;
    hm->size = size;
    hm->data = calloc(size, sizeof(struct pair_ast *));
    if (!hm->data)
        return NULL;
    return hm;
}

static struct pair_ast *is_key_in_ast(struct pair_ast **p, const char *key)
{
    while (*p)
    {
        if (!strcmp((*p)->key, key))
            return *p;
        *p = (*p)->next;
    }
    return NULL;
}

// 28 lines
bool f_hash_map_insert(struct f_hash_map *hash_map, const char *key, struct ast *value)
{
    if (!hash_map || hash_map->size == 0)
        return false;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == NULL)
    {
        struct pair_ast *q = malloc(sizeof(struct pair_ast));
        if (!q)
            return false;
        q->key = strdup(key);
        q->value = value;
        q->next = NULL;
        hash_map->data[i] = q;
    }
    else
    {
        struct pair_ast *cpy = hash_map->data[i];
        struct pair_ast *temp = is_key_in_ast(&hash_map->data[i], key);
        if (temp != NULL)
        {
            if (temp->value != NULL)
                free_func(temp->value, 0);
            temp->value = value;
        }
        else
        {
            hash_map->data[i] = cpy;
            struct pair_ast *p = malloc(sizeof(struct pair_ast));
            if (!p)
                return false;
            p->key = strdup(key);
            p->value = value;
            p->next = hash_map->data[i];
            hash_map->data[i] = p;
        }
    }
    return true;
}

static void free_pair_ast(struct pair_ast *pair, int force)
{
    free(pair->key);
    free_func(pair->value, force);
    free(pair);
}

// 10 lines
void f_hash_map_free(struct f_hash_map *hash_map)
{
    if (!hash_map)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_ast *l = hash_map->data[i];
        while (hash_map->data[i])
        {
            l = l->next;
            free_pair_ast(hash_map->data[i], 1);
            hash_map->data[i] = l;
        }
    }
    free(hash_map->data);
    free(hash_map);
}

// 12 lines
struct ast *f_hash_map_get(const struct f_hash_map *hash_map, const char *key)
{
    if (!hash_map || hash_map->size == 0)
        return NULL;
    struct pair_ast *q;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == 0)
        return NULL;
    q = hash_map->data[i];
    while (q)
    {
        if (!strcmp(q->key, key))
            return q->value;
        q = q->next;
    }
    return NULL;
}

// 32 lines
bool f_hash_map_remove(struct f_hash_map *hash_map, const char *key)
{
    if (!hash_map || hash_map->size == 0)
        return false;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == 0)
        return false;
    struct pair_ast *q;
    q = hash_map->data[i];
    if (!q->next)
    {
        if (!strcmp(q->key, key))
        {
            free(hash_map->data[i]->key);
            free_func(hash_map->data[i]->value, 0);
            free(hash_map->data[i]);
            hash_map->data[i] = NULL;
            return true;
        }
        return false;
    }
    if (!strcmp(q->key, key))
    {
        q = q->next;
        free(hash_map->data[i]->key);
        free_func(hash_map->data[i]->value, 0);
        free(hash_map->data[i]);
        hash_map->data[i] = q;
        return true;
    }
    struct pair_ast *old = q;
    q = q->next;
    while (q)
    {
        if (!strcmp(q->key, key))
        {
            old->next = q->next;
            free_pair_ast(q, 0);
            return true;
        }
        old = q;
        q = q->next;
    }
    return false;
}
