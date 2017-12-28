#ifndef __HASHMAP_HEADER__
#define __HASHMAP_HEADER__

#include <stdint.h>
#include <pthread.h>
#include "darray.h"

#define DEFAULT_NUMBER_OF_BUCKETS 100
#define LOAD_FACTOR (2.0 / 3.0)
#define HASHMAP_DESTROY_DATA 1
#define HASHMAP_DESTROY_KEYS 2


typedef int (*Hashmap_compare)(void *a, void *b);
typedef uint32_t (*Hashmap_hash)(void *key);


typedef struct Hashmap {
    Darray *buckets;
    Hashmap_compare compare;
    Hashmap_hash hash;
    pthread_mutex_t *mtx;
    int spacesOccupied;
    int numBuckets;
    uint32_t salt;
} Hashmap;


typedef struct HashmapNode {
    void *key;
    void *data;
    uint32_t hash;
} HashmapNode;


typedef int (*Hashmap_traverse_cb)(HashmapNode *node);

// pass in 0 for initialSize to set it to DEFAULT_NUMBER_OF_BUCKETS
Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash, size_t initialSize);

double Hashmap_load_factor(Hashmap *map);

int Hashmap_expand(Hashmap *map);

int Hashmap_contract(Hashmap *map);

uint32_t Hashmap_salt(void);

void Hashmap_destroy(Hashmap *map, int flags);

int Hashmap_set(Hashmap *map, void *key, void *data);

void *Hashmap_get(Hashmap *map, void *key);

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb);

void *Hashmap_delete(Hashmap *map, void *key);

#endif
