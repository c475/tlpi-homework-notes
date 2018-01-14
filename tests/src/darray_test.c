#include "../../lib/tlpi_hdr.h"
#include <pthread.h>

#define NUM_THREADS 40


void *threadFunc(void *arg)
{
    int i;
    int *val;
    Darray *array;

    array = (Darray *)arg;

    for (i = 0; i < 10000; i++) {
        val = malloc(sizeof(int));
        if (val == NULL) {
            errExit("malloc");
        }
        *val = i;
        Darray_push(array, val);
    }

    return NULL;
}


int push_pop_test(Darray *array)
{
    int i;
    int max;
    int *val;

    max = 10000;

    for (i = 0; i <= max; i++) {
        val = malloc(sizeof(int));
        if (val == NULL) {
            errExit("malloc");
        }
        *val = i;
        Darray_push(array, val);
    }

    for (i = 0; i <= max; i++) {
        val = Darray_pop(array);
        if (*val != max - i) {
            errExit("Darray_pop");
        }

        free(val);
    }

    return 0;
}


int get_set_test(Darray *array)
{
    int *val1;
    int *val2;

    val1 = malloc(sizeof(int));
    if (val1 == NULL) {
        errExit("malloc");
    }

    val2 = malloc(sizeof(int));
    if (val2 == NULL) {
        errExit("malloc");
    }

    *val1 = 1;
    *val2 = 2;

    Darray_set(array, 0, val1);
    Darray_set(array, 1, val2);

    if (Darray_get(array, 0) != val1) {
        errExit("Darray_get");
    }

    if (Darray_get(array, 1) != val2) {
        errExit("Darray_get");
    }

    free(Darray_pop(array));
    free(Darray_pop(array));

    return 0;
}


int expand_contract_test(Darray *array)
{
    int oldMax;

    oldMax = array->max;

    Darray_expand(array);

    if (array->max != oldMax + array->expandRate) {
        errExit("Darray_expand");
    }

    Darray_contract(array);

    if (array->max != array->expandRate + 1) {
        errExit("Darray_contract");
    }

    return 0;
}


int thread_test(Darray *array)
{
    int i;
    pthread_t *threads;

    threads = calloc(1, sizeof(pthread_t) * NUM_THREADS);
    if (threads == NULL) {
        errExit("calloc");
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, threadFunc, array) != 0) {
            errExit("pthread_create");
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            errExit("pthread_join");
        }
    }

    return 0;
}


int test_remove(Darray *array)
{
    int *val1;
    int *val2;

    val1 = malloc(sizeof(int));
    *val1 = 4000;

    val2 = malloc(sizeof(int));
    *val2 = 6000;

    Darray_push(array, val1);
    Darray_push(array, val2);

    if (Darray_remove(array, 1) != val2) {
        errExit("Darray_remove");
    }

    if (Darray_remove(array, 0) != val1) {
        errExit("Darray_remove");
    }

    if (Darray_end(array) != 2) {
        errExit("Darray_end() wrong value");
    }

    if (Darray_pop(array) != NULL) {
        errExit("Darray_pop() should return NULL");
    }

    if (Darray_pop(array) != NULL) {
        errExit("Darray_pop() should return NULL");
    }

    if (Darray_end(array) != 0) {
        errExit("Darray_end() should return 0");
    }

    free(val1);
    free(val2);

    return 0;
}

int shitload_of_values_test(Darray *array)
{
    int i;
    int *val;

    for (i = 0; i < 5000; i++) {
        val = calloc(1, sizeof(int));
        if (val == NULL) {
            errExit("calloc");
        }

        *val = i;

        Darray_push(array, val);
    }

    return 0;
}


int is_sorted_asc(Darray *array)
{
    int i;
    int *val1;
    int *val2;

    for (i = 0; i < Darray_count(array) - 1; i++) {
        val1 = Darray_get(array, i);
        val2 = Darray_get(array, i + 1);
        if (*val1 > *val2) {

            return 0;
        }
    }

    return 1;
}


int is_sorted_desc(Darray *array)
{
    int i;
    int *val1;
    int *val2;

    for (i = 0; i < Darray_count(array) - 1; i++) {
        val1 = Darray_get(array, i);
        val2 = Darray_get(array, i + 1);
        if (*val1 < *val2) {
            return 0;
        }
    }

    return 1;
}


int sort_asc(const void *a, const void *b)
{
    int *v1, *v2;

    v1 = (int *)a;
    v2 = (int *)b;

    return *v1 < *v2;
}


int sort_desc(const void *a, const void *b)
{
    int *v1, *v2;

    v1 = (int *)a;
    v2 = (int *)b;

    return *v1 > *v2;
}


int mergesort_test(Darray *array)
{
    Darray_mergesort(array, (Darray_compare)sort_asc);
    Darray_debug(array);
    assert(is_sorted_asc(array));
    return 0;
}


int heapsort_test(Darray *array)
{
    Darray_mergesort(array, (Darray_compare)sort_desc);
    Darray_debug(array);
    assert(is_sorted_desc(array));
    return 0;
}


int quicksort_test(Darray *array)
{
    Darray_mergesort(array, sort_asc);
    Darray_debug(array);
    return 0;
}


int main(int argc, char *argv[])
{
    Darray *array;

    array = Darray_create(sizeof(int), 100);
    if (array == NULL) {
        errExit("Darray_create");
    }

    push_pop_test(array);
    expand_contract_test(array);
    thread_test(array);
    shitload_of_values_test(array);
    mergesort_test(array);

    Darray_clear_destroy(array);

    exit(EXIT_SUCCESS);
}
