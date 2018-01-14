#include "../../lib/tlpi_hdr.h"
#include <pthread.h>
#include <assert.h>


#define NUM_THREADS 10
#define NUM_ITEMS_THREAD 10 // number of items each thread pushes into the list


int List_blaster_test(List *list)
{
    int *val;
    int i;

    for (i = 0; i < 10; i++) {
        val = calloc(1, sizeof(int));
        if (val == NULL) {
            errExit("List_blaster_test");
        }

        *val = i;
        List_push(list, val);
    }

    return 0;
}


int List_pop_test(List *list)
{
    int i;
    int *val;
    int count;

    count = list->count;

    for (i = 0; i < count; i++) {
        val = List_pop(list);
        free(val);
    }

    assert(List_count(list) == 0);
    assert(list->first == NULL);
    assert(list->last == NULL);

    return 0;
}


int List_pop_left_test(List *list)
{
    int i;
    int *val;
    int count;

    count = list->count;

    for (i = 0; i < count; i++) {
        val = List_pop_left(list);
        free(val);
    }

    assert(List_count(list) == 0);
    assert(list->first == NULL);
    assert(list->last == NULL);

    return 0;
}


int List_copy_test(List *list)
{
    List *copy;

    copy = List_copy(list);
    assert(copy);

    // demonstrate deepness of the copy
    *(int *)copy->first->next->value *= 2;
    assert(*(int *)list->first->next->value != *(int *)copy->first->next->value);

    List_destroy(copy);

    return 0;
}


int List_foreach_test(List *list, const char *msg)
{
    ListNode *node;
    if (msg != NULL) {
        printf("%s\n", msg);
    }

    node = list->first;
    while (node != NULL) {
        printf("%d\n", *(int *)node->value);
        node = node->next;
    }

    return 0;
}


List *List_split_test(List *list)
{
    int index;
    List *right;
    size_t oldCount;

    oldCount = list->count;
    index = 9;

    List_foreach_test(list, "Before split:");

    right = List_split(list, index);
    assert(right);
    assert(right->count == oldCount - index);
    assert(list->count == index);
    assert(right->first->prev == NULL);
    assert(list->last->next == NULL);
    assert(list->last != right->first);
    assert(list != right);

    return right;
}


int List_join_test(List *list, List *right)
{
    List *joined;
    size_t rightCount;
    size_t leftCount;

    leftCount = list->count;
    rightCount = right->count;

    joined = List_join(list, right);
    assert(joined);
    assert(joined == list);
    assert(list->count == leftCount + rightCount);

    List_foreach_test(list, "Printing out rejoined list");

    return 0;
}


void *threadFunc(void *arg)
{
    int i;
    int *val;
    List *list;

    list = (List *)arg;

    for (i = 0; i < NUM_ITEMS_THREAD; i++) {
        val = calloc(1, sizeof(int));
        *val = i;
        List_push(list, val);
    }

    return NULL;
}


int List_multithread_test(List *list)
{
    int i;
    pthread_t *threads;

    printf("POP TEST\n");
    // just get rid of all items in the list
    List_pop_test(list);
    printf("AFTER POP TEST\n");

    threads = calloc(NUM_THREADS, sizeof(pthread_t));
    assert(threads);

    for (i = 0; i < NUM_THREADS; i++) {
        assert(pthread_create(&threads[i], NULL, threadFunc, list) == 0);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        assert(pthread_join(threads[i], NULL) == 0);
    }

    assert(list->count == NUM_THREADS * NUM_ITEMS_THREAD);

    return 0;
}



int sort_desc(void *a, void *b)
{
    int *v1;
    int *v2;

    v1 = a;
    v2 = b;

    return *v1 > *v2;
}


int sort_asc(void *a, void *b)
{
    int *v1;
    int *v2;

    v1 = a;
    v2 = b;

    return *v1 < *v2;
}


int is_sorted_asc(List *list)
{
    ListNode *node;
    int *val1;
    int *val2;

    assert(list);

    node = list->first;
    while (node != NULL) {
        if (node->next == NULL) {
            break;
        }

        val1 = node->value;
        val2 = node->next->value;

        if (*val1 > *val2) {
            return 0;
        }

        node = node->next;
    }

    return 1;
}


int is_sorted_desc(List *list)
{
    ListNode *node;
    int *val1;
    int *val2;

    assert(list);

    node = list->first;
    while (node != NULL) {
        if (node->next == NULL) {
            break;
        }

        val1 = node->value;
        val2 = node->next->value;

        if (*val1 < *val2) {
            return 0;
        }

        node = node->next;
    }

    return 1;
}


int List_bubble_sort_test(List *list)
{
    List_bubble_sort(list, (List_compare)sort_asc);
    assert(is_sorted_asc(list));
    return 0;
}


int List_mergesort_test(List *list)
{
    List_mergesort(list, (List_compare)sort_desc);
    assert(is_sorted_desc(list));
    return 0;
}


int main(int argc, char *argv[])
{
    List *list;
    List *right;

    list = List_create(sizeof(int));
    if (list == NULL) {
        errExit("List_create");
    }

    List_blaster_test(list);
    List_pop_test(list);
    List_blaster_test(list);
    List_pop_left_test(list);
    List_blaster_test(list);
    right = List_split_test(list);
    List_join_test(list, right);
    List_copy_test(list);
    List_multithread_test(list);
    List_debug(list);
    List_bubble_sort_test(list);
    List_debug(list);
    List_mergesort_test(list);
    List_debug(list);

    List_destroy(list);

    exit(EXIT_SUCCESS);
}
