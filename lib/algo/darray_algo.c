#define _DEFAULT_SOURCE
#include "darray_algo.h"
#include <stdlib.h>
#include <bsd/stdlib.h>

#define COPY_INITIAL_MAX 300


int Darray_qsort(Darray *array, Darray_compare cmp)
{
    qsort(array->contents, Darray_count(array), sizeof(void *), cmp);
    return 0;
}


int Darray_heapsort(Darray *array, Darray_compare cmp)
{
    return heapsort(array->contents, Darray_count(array), sizeof(void *), cmp);
}



static void Darray_merge(Darray *left, Darray *right, Darray *array, int leftCount, int rightCount, Darray_compare cmp)
{
    int i;
    int k;
    int j;
    void *leftVal;
    void *rightVal;

    i = 0;
    j = 0;
    k = 0;

    while (i < leftCount && j < rightCount) {
        leftVal = Darray_get(left, i);
        rightVal = Darray_get(right, j);
        if (cmp(leftVal, rightVal) > 0) {
            Darray_set(array, k, leftVal);
            i++;
            k++;
        } else {
            Darray_set(array, k, rightVal);
            j++;
            k++;
        }
    }

    while (i < leftCount) {
        leftVal = Darray_get(left, i);
        Darray_set(array, k, leftVal);
        i++;
        k++;
    }

    while (j < rightCount) {
        rightVal = Darray_get(right, j);
        Darray_set(array, k, rightVal);
        j++;
        k++;
    }
}


static void Darray_merge_top_down(Darray *array, int count, Darray_compare cmp)
{
    int i;
    int middle;
    Darray *left;
    Darray *right;
    void *value;

    // base condition
    if (count < 2) {
        return;
    }

    middle = count / 2;

    left = Darray_create(array->elementSize, middle);
    right = Darray_create(array->elementSize, count - middle);

    for (i = 0; i < count; i++) {
        value = Darray_get(array, i);
        if (i < middle) {
            Darray_push(left, value);
        } else {
            Darray_push(right, value);
        }
    }

    Darray_merge_top_down(left, middle, cmp); // left run
    Darray_merge_top_down(right, count - middle, cmp); // right run
    Darray_merge(left, right, array, middle, count - middle, cmp); // merge left and right into the array as a sorted list

    Darray_destroy(left);
    Darray_destroy(right);
}


int Darray_mergesort(Darray *array, Darray_compare cmp)
{
    Darray_merge_top_down(array, Darray_count(array), cmp);
    return 0;
}
