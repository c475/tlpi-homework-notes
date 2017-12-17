#include "linked_list.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
    Generic doubly linked list implementation that is also thread safe
*/


struct List *listCreate(size_t elementSize)
{
    pthread_mutex_t *mtx;
    struct List *list;

    assert(elementSize > 0);

    list = calloc(1, sizeof(struct List));
    assert(list != NULL);

    mtx = calloc(1, sizeof(pthread_mutex_t));
    assert(mtx != NULL);

    list->elementSize = elementSize;
    list->first = NULL;
    list->last = NULL;
    list->mtx = mtx;

    if (pthread_mutex_init(list->mtx, NULL) != 0) {
        abort();
    }

    return list;
}


// data is copied to the node, so it can be stack allocated in the caller if they want
// otherwise if it is heap allocated, the caller is responsible for freeing the data once it is copied
struct Node *listCreateNode(struct List *list, void *data)
{
    struct Node *node;

    assert(list != NULL);

    node = calloc(1, sizeof(struct Node));
    assert(node != NULL);

    node->data = calloc(1, list->elementSize);
    assert(node->data != NULL);

    memcpy(node->data, data, list->elementSize);

    node->prev = NULL;
    node->next = NULL;

    return node;
}


void listAppend(struct List *list, void *data)
{
    struct Node *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = listCreateNode(list, data);
    assert(node != NULL);

    if (list->length == 0) {
        list->first = node;
        list->last = node;
    } else {
        node->prev = list->last;
        list->last->next = node;
        list->last = node;
    }

    list->length++;

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }
}


void listPrepend(struct List *list, void *data)
{
    struct Node *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = listCreateNode(list, data);
    assert(node != NULL);

    if (list->length == 0) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->length++;

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }
}


void listRemoveWhen(struct List *list, int (*deleteFunc)(void *data))
{
    int code;
    struct Node *node;
    struct Node *temp;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = list->first;
    while (node != NULL) {
        code = deleteFunc(node->data);

        if (code != 0) {
            temp = node->next;

            if (node->prev) {
                node->prev->next = node->next;
            } else {
                list->first = node->next;
            }

            if (node->next) {
                node->next->prev = node->prev;
            } else {
                list->last = node->prev;
            }

            free(node->data);
            free(node);

            list->length--;

            node = temp;

        } else {
            node = node->next;
        }
    }

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }
}


// thinking about it now it might be hairy destroying a linked list that is in active use by multiple threads
// the correct protocol would be to ensure that only a single thread is left operrating on the list before destroying it
void listDestroy(struct List *list)
{
    struct Node *node;
    struct Node *temp;

    assert(list != NULL);

    pthread_mutex_lock(list->mtx);

    node = list->first;
    while (node != NULL) {
        temp = node->next;
        free(node->data);
        free(node);
        node = temp;
    }

    pthread_mutex_destroy(list->mtx);
    free(list->mtx);
    free(list);
}


size_t listLength(struct List *list)
{
    size_t length;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    length = list->length;

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }

    return length;
}


size_t listSize(struct List *list)
{
    size_t size;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    size = list->length * list->elementSize;

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }

    return size;
}


void *listFindFirst(struct List *list, int (*searchFunc)(void *data))
{
    void *data;
    struct Node *node;

    assert(list != NULL);
    assert(searchFunc != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    data = NULL;
    node = list->first;
    while (node != NULL) {
        if (searchFunc(node->data)) {
            data = node->data;
            break;
        }

        node = node->next;
    }

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }

    return data;
}


// callback function return codes
// 0: continue
// 1: stop iteration
// can get more complex later like skipping elements, going back to elements, starting from the beginning, etc
// foreachFunc is free to mutate the data in the style of map()
void listForeach(struct List *list, int (*foreachFunc)(void *data, void *extra), void *extra)
{
    int code;
    struct Node *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = list->first;
    while (node != NULL) {
        code = foreachFunc(node->data, extra);
        if (code == 1) {
            break;
        }

        node = node->next;
    }

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }
}


struct List *listCopy(struct List *list)
{
    pthread_mutex_t *mtx;
    struct List *copy;
    struct Node *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    copy = calloc(1, sizeof(struct List));
    assert(copy != NULL);

    mtx = calloc(1, sizeof(pthread_mutex_t));
    assert(mtx != NULL);

    if (pthread_mutex_init(mtx, NULL) != 0) {
        abort();
    }

    copy->elementSize = list->elementSize;
    copy->mtx = mtx;
    copy->first = NULL;
    copy->last = NULL;

    node = list->first;
    while (node != NULL) {
        listAppend(copy, node->data);
        node = node->next;
    }

    if (pthread_mutex_unlock(list->mtx) != 0) {
        abort();
    }

    return copy;
}


void listRemove(struct List *list, void *data)
{
    struct Node *node;

    assert(list != NULL);
    assert(data != NULL);

    node = list->first;
    while (node != NULL) {
        if (node->data == data) {
            if (node->prev) {
                node->prev->next = node->next;
            } else {
                list->first = node->next;
            }

            if (node->next) {
                node->next->prev = node->prev;
            } else {
                list->last = node->prev;
            }

            free(node->data);
            free(node);
        }
    }
}


void *listPop(struct List *list, void *buffer)
{
    struct Node *node;

    assert(list != NULL);
    assert(buffer != NULL);

    node = list->last;
    if (node) {
        memcpy(buffer, node->data, list->elementSize);

        if (node->prev) {
            node->prev->next = NULL;
            list->last = node->prev;
        } else {
            list->first = NULL;
            list->last = NULL;
        }

        free(node->data);
        free(node);
    }

    return buffer;
}


void *listPopLeft(struct List *list, void *buffer)
{
    struct Node *node;

    assert(list != NULL);
    assert(buffer != NULL);

    node = list->first;
    if (node) {
        memcpy(buffer, node->data, list->elementSize);

        if (node->next) {
            node->next->prev = NULL;
            list->first = node->next;
        } else {
            list->first = NULL;
            list->last = NULL;
        }

        free(node->data);
        free(node);
    }

    return buffer;
}
