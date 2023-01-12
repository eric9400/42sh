#include <stdio.h>

#include "hash_map.h"

void hash_map_dump(struct hash_map *hash_map)
{
    struct pair_list *q;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        q = hash_map->data[i];
        while (q->next)
        {
            printf("%s: %s, ", q->key, q->value);
            q = q->next;
        }
        printf("%s: %s\n", q->key, q->value);
    }
}
