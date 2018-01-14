#ifndef __LINKED_LIST_HEADER__
#define __LINKED_LIST_HEADER__

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "darray.h"

#define DARRAY_INITIAL_MAX 300


typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    void *value;
} ListNode;

typedef struct List {
    int count;
    ListNode *first;
    ListNode *last;
    pthread_mutex_t *mtx;
    size_t elementSize;
} List;


List *List_create(size_t elementSize);
void List_destroy(List *list);
void List_clear(List *list);

#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)
#define List_size(A) ((A)->count * (A)->elementSize)

void List_push(List *list, void *value);
void List_push_deep(List *list, void *value); // same as push except makes a deep copy of the underlying value
void *List_pop(List *list);
void *List_pop_left(List *list);
List *List_copy(List *list); // perform deep copy of list
List *List_split(List *list, int index);
List *List_join(List *left, List *right);
Darray *List_to_darray(List *list);

void List_debug(List *list);

#endif
