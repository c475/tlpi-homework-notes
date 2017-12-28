#include "darray.h"
#include <string.h>
#include <stdio.h>


Darray *Darray_create(size_t elementSize, size_t initialMax)
{
    Darray *array;
    pthread_mutex_t *mtx;

    assert(elementSize > 0);
    assert(initialMax > 0);

    array = calloc(1, sizeof(Darray));
    if (array == NULL) {
        return NULL;
    }

    array->contents = calloc(initialMax, sizeof(void *));
    if (array->contents == NULL) {
        free(array);
        return NULL;
    }

    mtx = calloc(1, sizeof(pthread_mutex_t));
    if (mtx == NULL) {
        free(array->contents);
        free(array);
        return NULL;
    }

    if (pthread_mutex_init(mtx, NULL) != 0) {
        free(array->contents);
        free(array->mtx);
        free(array);
        return NULL;
    }

    array->end = 0;
    array->max = initialMax;
    array->elementSize = elementSize;
    array->expandRate = DEFAULT_EXPAND_RATE;
    array->mtx = mtx;

    return array;
}


void Darray_clear(Darray *array)
{
    int i;

    assert(array);

    if (array->elementSize > 0) {
        for (i = 0; i < array->max; i++) {
            if (array->contents[i] != NULL) {
                free(array->contents[i]);
            }
        }
    }
}


static inline int Darray_resize(Darray *array, size_t newSize)
{
    void *contents;

    assert(array);
    assert(newSize > 0);

    contents = realloc(array->contents, newSize * sizeof(void *));
    if (contents == NULL) {
        return -1;
    }

    array->max = newSize;
    array->contents = contents;

    return 0;
}


int Darray_expand(Darray *array)
{
    int rc;
    size_t oldMax;

    assert(array);

    oldMax = array->max;

    rc = Darray_resize(array, array->max + array->expandRate);
    if (rc == -1) {
        return -1;
    }

    // null out the new chunk
    memset(array->contents + oldMax, 0, array->expandRate * sizeof(void *));

    return 0;
}


int Darray_contract(Darray *array)
{
    int newSize;

    assert(array);

    if (array->end < (int)array->expandRate) {
        newSize = array->expandRate;
    } else {
        newSize = array->end;
    }

    return Darray_resize(array, newSize + 1);
}


void Darray_destroy(Darray *array)
{
    assert(array);

    if (array->contents) {
        free(array->contents);
    }

    free(array->mtx);
    free(array);
}


void Darray_clear_destroy(Darray *array)
{
    Darray_clear(array);
    Darray_destroy(array);
}


int Darray_push(Darray *array, void *element)
{
    int rc;

    assert(array);
    assert(element);
    assert(pthread_mutex_lock(array->mtx) == 0);

    rc = 0;

    array->contents[array->end] = element;
    array->end++;

    if (Darray_end(array) >= Darray_max(array)) {
        rc = Darray_expand(array);
    }

    assert(pthread_mutex_unlock(array->mtx) == 0);

    return rc;
}


void *Darray_pop(Darray *array)
{
    void *element;

    assert(array);

    if (array->end == 0) {
        return NULL;
    }

    assert(pthread_mutex_lock(array->mtx) == 0);

    element = Darray_remove(array, array->end - 1);
    array->end--;

    // kind of confusing at first but the second check safely assumes that the array had been expanded in past calls to push()
    if (Darray_end(array) > (int)array->expandRate && Darray_end(array) % array->expandRate) {
        Darray_contract(array);
    }

    assert(pthread_mutex_unlock(array->mtx) == 0);

    return element;
}


// shallow copy a Darray
Darray *Darray_copy(Darray *array)
{
    int i;
    Darray *new;
    void *data;

    assert(pthread_mutex_lock(array->mtx) == 0);

    new = Darray_create(array->elementSize, array->max - 1);
    if (new == NULL) {
        return NULL;
    }

    for (i = 0; i < array->max; i++) {
        data = Darray_get(array, i);
        if (data != NULL) {
            Darray_set(new, i, data);
        }
    }

    assert(pthread_mutex_unlock(array->mtx) == 0);

    return new;
}
