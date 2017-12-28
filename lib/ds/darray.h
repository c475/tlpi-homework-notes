#ifndef __D_ARRAY__
#define __D_ARRAY__

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "list.h"


// jacked from https://github.com/zedshaw/liblcthw
// mostly just added a mutex and locking/unlocking to the Darray struct and its functions for multithreaded use


// so, it's just a ledger of pointers that doesn't deal with allocation of actual chunks of memory
// only allocates space for the pointers to pointers
// this has the advantage of being able to be indexed directly (at random)
typedef struct Darray {
    int end;
    int max;
    size_t elementSize; // only matters for the algorithms
    size_t expandRate;
    void **contents;
    pthread_mutex_t *mtx;
} Darray;


Darray *Darray_create(size_t elementSize, size_t initialMax);

void Darray_destroy(Darray *array);

void Darray_clear(Darray *array);

int Darray_expand(Darray *array);

int Darray_contract(Darray *array);

int Darray_push(Darray *array, void *el);

void *Darray_pop(Darray *array);

void Darray_clear_destroy(Darray *array);


#define Darray_last(A) ((A)->contents[(A)->end - 1])
#define Darray_first(A) ((A)->contents[0])
#define Darray_end(A) ((A)->end)
#define Darray_count(A) Darray_end(A)
#define Darray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300


static inline void Darray_set(Darray *array, int i, void *el)
{
    assert(array);
    assert(array->max > i);
    assert(pthread_mutex_lock(array->mtx) == 0);

    if (i > array->end) {
        array->end = i;
    }

    array->contents[i] = el;

    assert(pthread_mutex_unlock(array->mtx) == 0);
}


static inline void *Darray_get(Darray *array, int i)
{
    void *contents;

    assert(array);
    assert(array->max > i);
    assert(pthread_mutex_lock(array->mtx) == 0);

    contents = array->contents[i];

    assert(pthread_mutex_unlock(array->mtx) == 0);

    return contents;
}

// cannot be safely called inside of a thread
// could make it so but I'm not going to go that far with it
// just stick to push()/pop()/set()/get() in a thread
// But, it can be ok if wrapped by another mutex protected data structure like a Hashmap
static inline void *Darray_remove(Darray *array, int i)
{
    void *el;

    assert(array);
    assert(array->end >= i);

    el = array->contents[i];
    array->contents[i] = NULL;

    return el;
}


static inline void *Darray_new(Darray *array)
{
    size_t size;

    assert(array);
    assert(array->elementSize > 0);
    // probably unnecessary, since elementSize is unlikely to ever change
    // (especially while running in a thread)
    assert(pthread_mutex_lock(array->mtx) == 0);

    size = array->elementSize;

    assert(pthread_mutex_unlock(array->mtx) == 0);

    return calloc(1, size);
}


#define Darray_free(E) (free(E))

#endif
