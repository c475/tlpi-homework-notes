#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stddef.h>
#include <pthread.h>
#include "darray.h"


typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    void *data;
} ListNode;


typedef struct List {
    size_t length;
    size_t elementSize;
    struct ListNode *first;
    struct ListNode *last;
    pthread_mutex_t *mtx; // threadsafety
} List;


List *List_create(size_t elementSize); // create a list with the size of the elements that go in it
ListNode *list_create_node(List *list, void *data); // internal call to wrap (void *) in a ListNode
void list_free_node(ListNode *node); // internal call for freeing a node (used in various rm calls and listDestroy())
void List_append(List *list, void *data); // append data to the list
void List_prepend(List *list, void *data); // prepend data to the list
void *List_pop(List *list, void *buffer); // pop the last element and place its contents in the user-supplied buffer
void *List_pop_left(List *list, void *buffer); // pop the first element and place its contents in the user-supplied buffer

/* 
    Not a very good solution but can't think of a better one right now. User is still free to manually search the list.

    deleteFunc callback return codes:
        - Return 0 from the callback to pass on the element
        - Return 1 from the callback to delete first matching element and stop.
        - Return 2 from the callback to delete the element, and continue searching for elements to delete
*/
void List_remove_when(List *list, int (*deleteFunc)(void *data));
void *List_remove(List *list, void *data);

void List_destroy(List *list); // destroy and free the list's resources
size_t List_length(List *list); // return the length of the list
size_t List_size(List *list); // return the list size in bytes (does not count internal bookkeeping, only length * elementSize)

/*
    Not a very good solution but can't think of a better one right now. User is still free to manually search the list.

    searchFunc callback return codes:
        - Return 0 to skip the element
        - Return 1 to return the matching element
*/
void *List_find_first(List *list, int (*searchFunc)(void *data, void *ex1), void *extra);

void *List_remove(List *list, void *data); // remove the node where node->data == data

/*
    foreachFunc return codes:
        - Return -1 to abort the process
        - Return 0 to continue iteration
        - Return 1 to halt iteration
*/
void List_foreach(List *list, int (*foreachFunc)(void *data, void *ex1), void *extra); // roundrobin the list with some function

List *List_copy(List *list); // make and return a copy of the list

Darray *List_to_darray(List *list);

#endif
