#ifndef __HASH_HEADER__
#define __HASH_HEADER__

#include <stdint.h>
#include <string.h>

#define FNV_PRIME 16777619;
#define FNV_OFFSET_BASIS 2166136261;
#define MOD_ADLER 65521;


uint32_t fnv1a_hash(void *data);

uint32_t adler32_hash(void *data);

uint32_t djb_hash(void *data);

#endif
