#include "hashmap.h"
#include <string.h>
#include <stdio.h>


static int default_compare(void *a, void *b)
{
    assert(a);
    assert(b);

    return strcmp((char *)a, (char *)b);
}


// jenkins hash
static uint32_t default_str_hash(void *a)
{
    size_t len;
    char *key;
    uint32_t hash;
    uint32_t i;

    assert(a);

    key = (char *)a;
    len = strlen(key);

    for (i = 0, hash = 0; i < len; i++) {
        hash += key[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}


Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash, size_t initialSize)
{
    Hashmap *map;
    int numBuckets;

    map = calloc(1, sizeof(Hashmap));
    if (map == NULL) {
        return NULL;
    }

    map->compare = compare == NULL ? default_compare : compare;
    map->hash = hash == NULL ? default_str_hash : hash;
    map->mtx = calloc(1, sizeof(pthread_mutex_t));
    if (map->mtx == NULL) {
        free(map);
        return NULL;
    }

    if (pthread_mutex_init(map->mtx, NULL) != 0) {
        free(map);
        return NULL;
    }

    numBuckets = initialSize > 0 ? initialSize : DEFAULT_NUMBER_OF_BUCKETS;

    map->buckets = Darray_create(sizeof(HashmapNode *), numBuckets);
    if (map->buckets == NULL) {
        free(map->mtx);
        free(map);
        return NULL;
    }

    map->numBuckets = numBuckets;
    map->buckets->end = map->buckets->max; // just pretend it is all taken up

    return map;
}


void Hashmap_destroy(Hashmap *map, int flags)
{
    int i;
    int j;
    Darray *bucket;
    HashmapNode *node;

    assert(map);
    assert(map->buckets);

    // A little awkward but makes sense
    for (i = 0; i < Darray_count(map->buckets); i++) {
        bucket = Darray_get(map->buckets, i);
        if (bucket != NULL) {
            // None of these are going to be NULL
            // remember, this is only freeing the node, not the data inside (node->data)
            for (j = 0; j < Darray_count(bucket); j++) {
                node = Darray_get(bucket, j);
                if (flags & HASHMAP_DESTROY_DATA) {
                    free(node->data);
                }

                if (flags & HASHMAP_DESTROY_KEYS) {
                    free(node->key);
                }

                free(node);
            }

            Darray_destroy(bucket);
        }
    }

    // destroy the array of pointers
    Darray_destroy(map->buckets);

    pthread_mutex_destroy(map->mtx);

    free(map->mtx);
    free(map);
}


static inline HashmapNode *Hashmap_node_create(int hash, void *key, void *data)
{
    HashmapNode *node;

    assert(key);

    node = calloc(1, sizeof(HashmapNode));
    assert(node);

    node->key = key;
    node->data = data;
    node->hash = hash;

    return node;
}


static inline Darray *Hashmap_find_bucket(Hashmap *map, void *key, int create, uint32_t *hash_out)
{
    Darray *bucket;
    uint32_t hash;
    int bucketIndex;

    assert(map);

    hash = map->hash(key);
    *hash_out = hash;
    bucketIndex = hash % map->numBuckets;

    assert(bucketIndex >= 0);

    bucket = Darray_get(map->buckets, bucketIndex);
    if (bucket == NULL && create) {
        // new bucket, set it up
        bucket = Darray_create(sizeof(void *), map->numBuckets);
        assert(bucket);
        Darray_set(map->buckets, bucketIndex, bucket);
    }

    return bucket;
}


int Hashmap_set(Hashmap *map, void *key, void *data)
{
    uint32_t hash;
    Darray *bucket;
    HashmapNode *node;

    assert(map);
    assert(key);

    assert(pthread_mutex_lock(map->mtx) == 0);

    bucket = Hashmap_find_bucket(map, key, 1, &hash);
    assert(bucket);

    node = Hashmap_node_create(hash, key, data);
    assert(node);

    // push new node into the bucket's "chain"
    Darray_push(bucket, node);

    assert(pthread_mutex_unlock(map->mtx) == 0);

    return 0;
}


static inline int Hashmap_get_node(Hashmap *map, uint32_t hash, Darray *bucket, void *key)
{
    int i;
    HashmapNode *node;

    for (i = 0; i < Darray_end(bucket); i++) {
        node = Darray_get(bucket, i);
        if (node->hash == hash && map->compare(node->key, key) == 0) {
            return i;
        }
    }

    return -1;
}


void *Hashmap_get(Hashmap *map, void *key)
{
    int i;
    uint32_t hash;
    Darray *bucket;
    HashmapNode *node;

    assert(pthread_mutex_lock(map->mtx) == 0);

    bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (bucket == NULL) {
        return NULL;
    }

    // get the index of the node in the "chain"
    i = Hashmap_get_node(map, hash, bucket, key);
    if (i == -1) {
        return NULL;
    }

    node = Darray_get(bucket, i);
    assert(node);

    assert(pthread_mutex_unlock(map->mtx) == 0);

    return node->data;
}


int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb)
{
    int i;
    int j;
    int rc;
    Darray *bucket;
    HashmapNode *node;

    assert(pthread_mutex_lock(map->mtx) == 0);

    for (i = 0; i < Darray_count(map->buckets); i++) {
        bucket = Darray_get(map->buckets, i);

        if (bucket) {
            for (j = 0; j < Darray_count(bucket); j++) {
                node = Darray_get(bucket, j);
                rc = traverse_cb(node);

                if (rc != 0) {
                    assert(pthread_mutex_unlock(map->mtx) == 0);
                    return rc;
                }
            }
        }
    }

    assert(pthread_mutex_unlock(map->mtx) == 0);

    return 0;
}


void *Hashmap_delete(Hashmap *map, void *key)
{
    int i;
    uint32_t hash;
    Darray *bucket;
    HashmapNode *node;
    HashmapNode *ending;
    void *data;

    assert(pthread_mutex_lock(map->mtx) == 0);

    hash = 0;

    bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (bucket == NULL) {
        return NULL;
    }

    i = Hashmap_get_node(map, hash, bucket, key);
    if (i == -1) {
        return NULL;
    }

    node = Darray_get(bucket, i);
    assert(node);

    ending = Darray_pop(bucket);

    // just replace the deleted node with the popped node (if the deleted node wasn't popped)
    if (ending != node) {
        Darray_set(bucket, i, ending);
    }

    data = node->data;
    free(node);

    assert(pthread_mutex_unlock(map->mtx) == 0);

    return data;
}


// jenkins hash
static inline uint32_t Hashmap_uint32_hash(void *a)
{
    uint32_t val;
    uint32_t hash;

    assert(a);

    val = *(uint32_t *)a;
    hash = 0;

    while (val) {
        hash += val & 0xFF;
        hash += hash << 10;
        hash ^= hash >> 6;
        val >>= 8;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}
