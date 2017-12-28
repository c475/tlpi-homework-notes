#include "hash.h"


uint32_t fnv1a_hash(void *data)
{
    int i;
    char *str;
    uint32_t hash;

    hash = FNV_OFFSET_BASIS;
    str = (char *)data;

    for (i = 0; i < strlen(str); i++) {
        hash ^= str[i];
        hash *= FNV_PRIME;
    }

    return hash;
}


uint32_t adler32_hash(void *data)
{
    int i;
    char *str;
    uint32_t a;
    uint32_t b;

    a = 1;
    b = 0;
    str = (char *)data;

    for (i = 0; i < strlen(str); i++) {
        a = (a + str[i]) % MOD_ADLER;
        b = (a + b) % MOD_ADLER;
    }

    return (b << 16) | a;
}


uint32_t djb_hash(void *data)
{
    int i;
    uint32_t hash;
    char *str;

    hash = 5381;
    str = (char *)data;
    for (i = 0; i < strlen(str); i++) {
        hash = ((hash << 5) + hash) + str[i]; // hash * 33 + c
    }

    return hash;
}
