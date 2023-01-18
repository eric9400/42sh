#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

struct pair_list
{
    char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

struct hash_map *hash_map_init(size_t size);
void hash_map_free(struct hash_map *hash_map);

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value);
const char *hash_map_get(const struct hash_map *hash_map, const char *key);
bool hash_map_remove(struct hash_map *hash_map, const char *key);

extern struct hash_map *hashmap;

char *hashmap_get_copy(struct hash_map *hashmap, char *hkey);

#endif /* !HASH_MAP_H */
