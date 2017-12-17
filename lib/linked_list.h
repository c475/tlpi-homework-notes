#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stddef.h>
#include <pthread.h>

struct List {
    size_t length;
    size_t elementSize;
    struct Node *first;
    struct Node *last;
    pthread_mutex_t *mtx; // threadsafety
};

struct Node {
    struct Node *prev;
    struct Node *next;
    void *data;
};

struct List *listCreate(size_t elementSize); // create a list with the size of the elements that go in it
struct Node *listCreateNode(struct List *list, void *data); // internal call to wrap (void *) in a struct Node
void listFreeNode(struct Node *node); // internal call for freeing a node (used in various rm calls and listDestroy())
void listAppend(struct List *list, void *data); // append data to the list
void listPrepend(struct List *list, void *data); // prepend data to the list
void *listPop(struct List *list, void *buffer); // pop the last element and place its contents in the user-supplied buffer
void *listPopLeft(struct List *list, void *buffer); // pop the first element and place its contents in the user-supplied buffer

/* 
    Not a very good solution but can't think of a better one right now. User is still free to manually search the list.

    deleteFunc callback return codes:
        - Return 0 from the callback to pass on the element
        - Return 1 from the callback to delete first matching element and stop.
        - Return 2 from the callback to delete the element, and continue searching for elements to delete
*/
void listRemoveWhen(struct List *list, int (*deleteFunc)(void *data));
void listRemove(struct List *list, void *data);

void listDestroy(struct List *list); // destroy and free the list's resources
size_t listLength(struct List *list); // return the length of the list
size_t listSize(struct List *list); // return the list size in bytes (does not count internal bookkeeping, only length * elementSize)

/*
    Not a very good solution but can't think of a better one right now. User is still free to manually search the list.

    searchFunc callback return codes:
        - Return 0 to skip the element
        - Return 1 to return the matching element
*/
void *listFindFirst(struct List *list, int (*searchFunc)(void *data, void *extra), void *extra);

void listRemove(struct List *list, void *data); // remove the node where node->data == data

/*
    foreachFunc return codes:
        - Return -1 to abort the process
        - Return 0 to continue iteration
        - Return 1 to halt iteration
*/
void listForeach(struct List *list, int (*foreachFunc)(void *data, void *ex1), void *extra); // roundrobin the list with some function

struct List *listCopy(struct List *list); // make and return a copy of the list

#endif
