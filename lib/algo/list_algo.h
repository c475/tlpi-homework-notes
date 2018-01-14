#ifndef __LIST_ALGO_HEADER__
#define __LIST_ALGO_HEADER__

#include "../ds/list.h"
#include "darray_algo.h"
#include "../ds/darray.h"

typedef int (*List_compare)(void *a, void *b);

int List_bubble_sort(List *list, List_compare cmp);
int List_mergesort(List *list, List_compare cmp);

#endif
