#include "list.h"
#include <stdio.h>


List *List_create(size_t elementSize)
{
    List *list;
    pthread_mutex_t *mtx;

    assert(elementSize > 0);

    list = calloc(1, sizeof(List));
    if (list == NULL) {
        return NULL;
    }

    mtx = calloc(1, sizeof(pthread_mutex_t));
    if (mtx == NULL) {
        return NULL;
    }

    if (pthread_mutex_init(mtx, NULL) != 0) {
        free(list);
        free(mtx);
        return NULL;
    }

    list->mtx = mtx;
    list->elementSize = elementSize;

    return list;
}


void *List_remove(List *list, ListNode *node)
{
    void *result;
    ListNode *next;
    ListNode *prev;

    assert(list);
    assert(node);
    assert(list->first && list->last);

    if (node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if (node == list->first) {
        list->first = node->next;
        assert(list->first);
        list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        assert(list->last);
        list->last->next = NULL;
    } else {
        next = node->next;
        prev = node->prev;
        next->prev = prev;
        prev->next = next;
    }

    list->count--;
    result = node->value;
    free(node);

    return result;
}


// should destroy the list container, along with all nodes and their values
void List_destroy(List *list)
{
    List_clear(list);
    free(list->mtx);
    free(list);
}


// should clear a list to len=0
void List_clear(List *list)
{
    void *value;
    ListNode *node;
    ListNode *next;

    assert(list);

    node = list->first;
    while (node != NULL) {
        next = node->next;
        value = List_remove(list, node);
        if (value) {
            free(value);
        }

        node = next;
    }
}


void List_push(List *list, void *value)
{
    ListNode *node;

    assert(list);
    assert(pthread_mutex_lock(list->mtx) == 0);

    node = calloc(1, sizeof(ListNode));
    assert(node);

    node->value = value;

    if (list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

    assert(pthread_mutex_unlock(list->mtx) == 0);
}


void List_push_deep(List *list, void *value)
{
    ListNode *node;

    assert(list);
    assert(pthread_mutex_lock(list->mtx) == 0);

    node = calloc(1, sizeof(ListNode));
    assert(node);

    node->value = calloc(1, list->elementSize);
    assert(node->value);

    memcpy(node->value, value, list->elementSize);

    if (list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

    assert(pthread_mutex_unlock(list->mtx) == 0);
}



void *List_pop(List *list)
{
    void *value;
    ListNode *node;

    assert(list);
    assert(pthread_mutex_lock(list->mtx) == 0);

    node = list->last;
    value = node != NULL ? List_remove(list, node) : NULL;

    assert(pthread_mutex_unlock(list->mtx) == 0);

    return value;
}


void *List_pop_left(List *list)
{
    void *value;
    ListNode *node;

    assert(list);
    assert(pthread_mutex_lock(list->mtx) == 0);

    node = list->first;
    value = node != NULL ? List_remove(list, node) : NULL;

    assert(pthread_mutex_unlock(list->mtx) == 0);

    return value;
}


void List_debug(List *list)
{
    int i;
    ListNode *node;

    assert(list);

    node = list->first;
    i = 0;
    printf("==========================================================================================================================\n");
    printf("idx\t\tList\t\t\tNode\t\t\tPrev\t\t\tNext\t\t\tValue\n");
    printf("==========================================================================================================================\n");
    while (node != NULL) {
        printf("%6d\t\t0x%7lx\t\t0x%7lx\t\t0x%7lx\t\t0x%7lx\t\t%d\n", i, (long)list, (long)node, (long)node->prev, (long)node->next, *(int *)node->value);
        node = node->next;
        i++;
    }
    printf("==========================================================================================================================\n");
}


List *List_copy(List *list)
{
    List *copy;
    ListNode *node;

    assert(list);
    assert(pthread_mutex_lock(list->mtx) == 0);

    copy = List_create(list->elementSize);
    if (copy == NULL) {
        return NULL;
    }

    node = list->first;
    while (node != NULL) {
        List_push_deep(copy, node->value);
        node = node->next;
    }

    assert(pthread_mutex_unlock(list->mtx) == 0);

    return copy;
}


List *List_split(List *list, int index)
{
    int i;
    List *new;
    ListNode *node;

    assert(list);
    assert(index >= 0);
    assert(pthread_mutex_lock(list->mtx) == 0);

    if (index == 0 || index >= list->count) {
        return NULL;
    }

    new = List_create(list->elementSize);
    if (new == NULL) {
        return NULL;
    }

    // splitting off the second to first item
    if (index == 1) {
        new->first = list->first->next;
        new->first->prev = NULL;
        new->last = list->last;
        new->count = list->count - index;

        list->first->next = NULL;
        list->last = list->first;
        list->count = index;

    // splitting off the last item
    } else if (index == list->count - 1) {

        new->first = list->last;
        new->last = list->last;

        list->last = list->last->prev;

        new->last->next = NULL;
        new->last->prev = NULL;
        new->count = 1;

        list->last->next = NULL;
        list->count = index;


    // splitting off somewhere in the middle
    } else {
        // find the splitting point
        node = list->first->next;
        for (i = 1; i < index; i++) {
            node = node->next;
            assert(node);
        }

        new->count = list->count - index;
        new->first = node;
        new->last = list->last;

        list->count = index;
        list->last = new->first->prev;
        list->last->next = NULL;

        new->first->prev = NULL;
        
    }

    assert(pthread_mutex_unlock(list->mtx) == 0);

    return new;
}


// join the right hand list to the left hand list
// the right hand side is freed and should NOT be reused after being joined to the left hand side
List *List_join(List *left, List *right)
{
    assert(left);
    assert(right);

    if (left == right) {
        return left;
    }

    assert(pthread_mutex_lock(left->mtx) == 0);

    right->first->prev = left->last;
    left->last->next = right->first;
    left->last = right->last;
    left->count += right->count;

    // get rid of the right hand side
    pthread_mutex_destroy(right->mtx);
    free(right->mtx);
    free(right);

    assert(pthread_mutex_unlock(left->mtx) == 0);

    return left;
}


Darray *List_to_darray(List *list)
{
    Darray *array;
    ListNode *node;

    assert(list);

    array = Darray_create(list->elementSize, DARRAY_INITIAL_MAX);
    if (array == NULL) {
        return NULL;
    }

    node = list->first;
    while (node != NULL) {
        Darray_push(array, node->value);
        node = node->next;
    }

    return array;
}
