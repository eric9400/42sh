#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

struct pair_list {
  char *key;
  char *value;
  struct pair_list *next;
};

struct hash_map {
  struct pair_list **data;
  size_t size;
};

struct pair_ast {
  char *key;
  struct ast *value;
  struct pair_ast *next;
};

struct f_hash_map {
  struct pair_ast **data;
  size_t size;
};

struct hash_maps {
  struct hash_map *hashmap;
  struct f_hash_map *fhashmap;
};

extern struct hash_maps *hashM;

struct hash_map *hash_map_init(size_t size);
void hash_map_free(struct hash_map *hash_map);
bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value);
char *hash_map_get(const struct hash_map *hash_map, const char *key);
bool hash_map_remove(struct hash_map *hash_map, const char *key);
char *hashmap_get_copy(struct hash_map *hashmap, char *hkey);
char *hashmap_get_global(struct hash_map *hashmap, char *hkey);

struct f_hash_map *f_hash_map_init(size_t size);
bool f_hash_map_insert(struct f_hash_map *hash_map, const char *key,
                       struct ast *value);
void f_hash_map_free(struct f_hash_map *hash_map);
struct ast *f_hash_map_get(const struct f_hash_map *hash_map, const char *key);
bool f_hash_map_remove(struct f_hash_map *hash_map, const char *key);

#endif /* !HASH_MAP_H */
