#include "list_algo.h"
#include <stdio.h>


// just convert to Darray
int List_bubble_sort(List *list, List_compare cmp)
{
    int i;
    int j;
    int sorted;
    void *tmp;
    Darray *array;
    ListNode *node;

    if (list == NULL) {
        return -1;
    }

    if (cmp == NULL) {
        return -1;
    }

    // already sorted
    if (list->count <= 1) {
        return 0;
    }

    array = List_to_darray(list);
    if (array == NULL) {
        return -1;
    }

    for (i = 1; i < List_count(list); i++) {
        sorted = 1;
        for (j = 1; j < List_count(list); j++) {
            if (cmp(array->contents[j-1], array->contents[j]) <= 0) {
                tmp = array->contents[j-1];
                array->contents[j-1] = array->contents[j];
                array->contents[j] = tmp;
                sorted = 0;
            }
        }

        if (sorted) {
            break;
        }
    }

    for (i = 0, node = list->first; i < List_count(list); i++, node = node->next) {
        node->value = array->contents[i];
    }

    Darray_destroy(array);

    return 0;
}


// just do a darray sort instead and relink the list
int List_mergesort(List *list, List_compare cmp)
{
    int i;
    Darray *array;
    ListNode *node;

    if (list == NULL) {
        return -1;
    }

    if (cmp == NULL) {
        return -1;
    }

    array = List_to_darray(list);
    if (array == NULL) {
        return -1;
    }

    Darray_mergesort(array, (Darray_compare)cmp);

    for (i = 0, node = list->first; i < list->count; i++, node = node->next) {
        node->value = array->contents[i];
    }

    Darray_destroy(array);

    return 0;
}
