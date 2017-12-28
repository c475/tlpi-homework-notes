#include "../../lib/tlpi_hdr.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static char *key1 = "heytesting";
static char *key2 = "silenthorse";
static char *key3 = "dogigloo";

#define BUCKETS 1000
#define BUFFER_LEN 20
#define NUM_KEYS (BUCKETS * 1000)


#define NUM_HASH_TESTS 3
enum {
    ALGO_FNV1A,
    ALGO_ADLER32,
    ALGO_DJB
};


int fnv1a_test(void)
{
    uint32_t hash;

    hash = fnv1a_hash(key1);
    assert(hash != 0);

    hash = fnv1a_hash(key2);
    assert(hash != 0);

    hash = fnv1a_hash(key3);
    assert(hash != 0);

    return 0;
}


int adler32_test(void)
{
    uint32_t hash;

    hash = adler32_hash(key1);
    assert(hash != 0);

    hash = adler32_hash(key2);
    assert(hash != 0);

    hash = adler32_hash(key3);
    assert(hash != 0);

    return 0;
}


int djb_test(void)
{
    uint32_t hash;

    hash = djb_hash(key1);
    assert(hash != 0);

    hash = djb_hash(key2);
    assert(hash != 0);

    hash = djb_hash(key3);
    assert(hash != 0);

    return 0;
}


int gen_keys(Darray *keys, int numKeys)
{
    int i;
    int fd;
    int rc;
    uint8_t *buffer;

    fd = open("/dev/urandom", O_RDONLY);
    assert(fd != -1);

    assert(buffer != NULL);

    for (i = 0; i < numKeys; i++) {
        buffer = calloc(1, BUFFER_LEN);
        assert(buffer != NULL);

        rc = read(fd, buffer, BUFFER_LEN - 1);
        assert(rc != -1);

        Darray_push(keys, buffer);
    }

    close(fd);

    return 0;
}


void destroy_keys(Darray *keys)
{
    int i;

    for (i = 0; i < NUM_KEYS; i++) {
        free(Darray_get(keys, i));
    }

    Darray_destroy(keys);
}


void fill_distribution(int *stats, Darray *keys, uint32_t (*hash_func)(void *data))
{
    int i;
    uint32_t hash;

    for (i = 0; i < Darray_count(keys); i++) {
        hash = hash_func(Darray_get(keys, i));
        stats[hash % BUCKETS] += 1;
    }
}


// can take the produced table into R and analyze the output, pretty cool
int test_distribution(void)
{
    int i;
    int stats[NUM_HASH_TESTS][BUCKETS] = { {0} };
    Darray *keys;

    keys = Darray_create(sizeof(uint8_t *), NUM_KEYS);
    assert(keys != NULL);

    gen_keys(keys, NUM_KEYS);

    fill_distribution(stats[ALGO_FNV1A], keys, fnv1a_hash);
    fill_distribution(stats[ALGO_ADLER32], keys, adler32_hash);
    fill_distribution(stats[ALGO_DJB], keys, djb_hash);

    fprintf(stderr, "FNV\tA32\tDJB\n");

    for (i = 0; i < BUCKETS; i++) {
        fprintf(stderr, "%d\t%d\t%d\n", stats[ALGO_FNV1A][i], stats[ALGO_ADLER32][i], stats[ALGO_DJB][i]);
    }

    destroy_keys(keys);

    return 0;
}


int main(int argc, char *argv[])
{
    fnv1a_test();
    adler32_test();
    djb_test();
    test_distribution();

    exit(EXIT_SUCCESS);
}
