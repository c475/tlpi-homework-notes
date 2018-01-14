#ifndef __DARRAY_ALGO_HEADER__
#define __DARRAY_ALGO_HEADER__

#include "../ds/darray.h"

typedef int (*Darray_compare)(const void *a, const void *b);

int Darray_qsort(Darray *array, Darray_compare cmp);
int Darray_heapsort(Darray *array, Darray_compare cmp);
int Darray_mergesort(Darray *array, Darray_compare cmp);

#endif
