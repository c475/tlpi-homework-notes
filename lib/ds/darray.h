#ifndef __D_ARRAY__
#define __D_ARRAY__

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>


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


typedef int (*Darray_filter_cb)(void *value);


Darray *Darray_create(size_t elementSize, size_t initialMax);

void Darray_destroy(Darray *array);

void Darray_clear(Darray *array);

int Darray_expand(Darray *array);

int Darray_contract(Darray *array);

int Darray_push(Darray *array, void *el);

void *Darray_pop(Darray *array);

void Darray_clear_destroy(Darray *array);

Darray *Darray_copy(Darray *array);

Darray *Darray_filter(Darray *array, Darray_filter_cb cb);

void Darray_debug(Darray *array);


#define Darray_last(A) ((A)->contents[(A)->end - 1])
#define Darray_first(A) ((A)->contents[0])
#define Darray_end(A) ((A)->end)
#define Darray_count(A) Darray_end(A)
#define Darray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300
#define DARRAY_DEFAULT_SIZE 100


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


static inline void *Darray_remove(Darray *array, int i)
{
    void *el;

    assert(array);
    assert(array->end >= i);

    el = array->contents[i];
    array->contents[i] = NULL;

    return el;
}


#define Darray_free(E) (free(E))

#endif
