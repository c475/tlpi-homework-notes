#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
    Generic doubly linked list implementation that is also thread safe
*/


List *List_create(size_t elementSize)
{
    pthread_mutex_t *mtx;
    List *list;

    assert(elementSize > 0);

    list = calloc(1, sizeof(List));
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


ListNode *List_create_node(List *list, void *data, int deep)
{
    ListNode *node;

    assert(list != NULL);

    node = calloc(1, sizeof(ListNode));
    assert(node != NULL);

    if (deep) {
        node->data = calloc(1, list->elementSize);
        memcpy(node->data, data, list->elementSize);
    } else {
        node->data = data;
    }

    node->prev = NULL;
    node->next = NULL;

    return node;
}


void List_append(List *list, void *data)
{
    ListNode *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = List_create_node(list, data, 0);
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


void List_append_deep(List *list, void *data)
{
    ListNode *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = List_create_node(list, data, 1);
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


void List_prepend(List *list, void *data)
{
    ListNode *node;

    assert(list != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    node = List_create_node(list, data, 0);
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


void List_remove_when(List *list, int (*deleteFunc)(void *data))
{
    int code;
    ListNode *node;
    ListNode *temp;

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
void List_destroy(List *list)
{
    ListNode *node;
    ListNode *temp;

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


size_t List_length(List *list)
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


size_t List_size(List *list)
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


void *List_find_first(List *list, int (*searchFunc)(void *data, void *ex1), void *extra)
{
    void *data;
    ListNode *node;

    assert(list != NULL);
    assert(searchFunc != NULL);

    if (pthread_mutex_lock(list->mtx) != 0) {
        abort();
    }

    data = NULL;
    node = list->first;
    while (node != NULL) {
        if (searchFunc(node->data, extra)) {
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
void List_foreach(List *list, int (*foreachFunc)(void *data, void *ex1), void *extra)
{
    int code;
    ListNode *node;

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


// deep or shallow copy a list
List *List_copy(List *list, int deep)
{
    pthread_mutex_t *mtx;
    List *copy;
    ListNode *node;

    assert(list != NULL);
    assert(pthread_mutex_lock(list->mtx) == 0);

    copy = calloc(1, sizeof(List));
    assert(copy != NULL);

    mtx = calloc(1, sizeof(pthread_mutex_t));
    assert(mtx != NULL);

    assert(pthread_mutex_init(mtx, NULL) == 0);

    copy->elementSize = list->elementSize;
    copy->mtx = mtx;
    copy->first = NULL;
    copy->last = NULL;

    node = list->first;
    while (node != NULL) {
        if (deep) {
            List_append_deep(copy, node->data);
        } else {
            List_append(copy, node->data);
        }

        node = node->next;
    }

    assert(pthread_mutex_unlock(list->mtx) == 0);

    return copy;
}


// Handy for running certain algorithms that work better on an array (i.e. algos that want to index directly)
// Might have to be careful of the way you destroy the returned Darray... especially if you plan to still use the list.
// Same goes for the List as well if you plan on keeping the Darray around
Darray *List_to_darray(List *list)
{
    Darray *array;
    ListNode *node;

    array = Darray_create(sizeof(ListNode), list->length);
    if (array == NULL) {
        return NULL;
    }

    node = list->first;
    while (node != NULL) {
        Darray_push(array, node);
    }

    return array;
}


void *List_remove(List *list, void *data)
{
    void *result;
    ListNode *node;

    assert(list != NULL);
    assert(data != NULL);

    node = list->first;
    result = NULL;
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

            result = node->data;

            free(node);

            break;
        }
    }

    return result;
}


void *List_pop(List *list, void *buffer)
{
    ListNode *node;

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


void *List_pop_left(List *list, void *buffer)
{
    ListNode *node;

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
