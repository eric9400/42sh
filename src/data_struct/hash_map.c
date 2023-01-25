#define _XOPEN_SOURCE 600

#include "hash_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "ast_free.h"
#include "builtin.h"

static size_t hash(const char *key)
{
    size_t i = 0;
    size_t hash = 0;

    for (i = 0; key[i] != '\0'; ++i)
        hash += key[i];
    hash += i;

    return hash;
}

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *hm = malloc(sizeof(struct hash_map));
    if (!hm)
        return NULL;
    hm->size = size;
    hm->data = calloc(size, sizeof(struct pair_list *));
    if (!hm->data)
        return NULL;
    return hm;
}

static struct pair_list *is_key_in(struct pair_list **p, const char *key)
{
    while (*p)
    {
        if (!strcmp((*p)->key, key))
            return *p;
        *p = (*p)->next;
    }
    return NULL;
}

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value)
{
    if (getenv(key))
    {
	    char *str = calloc((strlen(key) + strlen(value) + 2), sizeof(char));
	    str = strcat(str, key);
	    str = strcat(str, "=");
	    str = strcat(str, value);
	    str = strcat(str, "\0");
	    export_insert(str);
	    free(str);
	    // maybe free TODO
    }
    if (!hash_map || hash_map->size == 0)
        return false;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == NULL)
    {
        struct pair_list *q = malloc(sizeof(struct pair_list));
        if (!q)
            return false;
        q->key = strdup(key);
        q->value = strdup(value);
        q->next = NULL;
        hash_map->data[i] = q;
    }
    else
    {
        struct pair_list *cpy = hash_map->data[i];
        struct pair_list *temp = is_key_in(&hash_map->data[i], key);
        if (temp != NULL)
        {
            if (temp->value != NULL)
                free(temp->value);
            temp->value = strdup(value);
        }
        else
        {
            hash_map->data[i] = cpy;
            struct pair_list *p = malloc(sizeof(struct pair_list));
            if (!p)
                return false;
            p->key = strdup(key);
            p->value = strdup(value);
            p->next = hash_map->data[i];
            hash_map->data[i] = p;
        }
    }
    return true;
}

static void free_pair(struct pair_list *pair)
{
    free(pair->key);
    free(pair->value);
    free(pair);
}

void hash_map_free(struct hash_map *hash_map)
{
    if (!hash_map)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *l = hash_map->data[i];
        while (hash_map->data[i])
        {
            l = l->next;
            free_pair(hash_map->data[i]);
            hash_map->data[i] = l;
        }
    }
    free(hash_map->data);
    free(hash_map);
}

char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    if (!hash_map || hash_map->size == 0)
        return NULL;
    struct pair_list *q;
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

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (!hash_map || hash_map->size == 0)
        return false;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == 0)
        return false;
    struct pair_list *q;
    q = hash_map->data[i];
    if (!q->next)
    {
        if (!strcmp(q->key, key))
        {
            free(hash_map->data[i]->key);
            free(hash_map->data[i]->value);
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
        free(hash_map->data[i]->value);
        free(hash_map->data[i]);
        hash_map->data[i] = q;
        return true;
    }
    struct pair_list *old = q;
    q = q->next;
    while (q)
    {
        if (!strcmp(q->key, key))
        {
            old->next = q->next;
            free(q);
            return true;
        }
        old = q;
        q = q->next;
    }
    return false;
}

char *hashmap_get_copy(struct hash_map *hashmap, char *hkey)
{
    const char *res = hash_map_get(hashmap, hkey);
    if (res == NULL)
        return NULL;
    return strdup(res);
}

//F_HASH_MAP

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
                free_func(temp->value);
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

static void free_pair_ast(struct pair_ast *pair)
{
    free(pair->key);
    free_func(pair->value);
    free(pair);
}

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
            free_pair_ast(hash_map->data[i]);
            hash_map->data[i] = l;
        }
    }
    free(hash_map->data);
    free(hash_map);
}

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
            free_func(hash_map->data[i]->value);
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
        free_func(hash_map->data[i]->value);
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
            free(q);
            return true;
        }
        old = q;
        q = q->next;
    }
    return false;
}
