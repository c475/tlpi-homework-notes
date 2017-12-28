#ifndef __LIST_ALGO_HEADER__
#define __LIST_ALGO_HEADER__


typedef int (*List_compare)(void *x, void *y);


static inline void List_swap_nodes(ListNode *n1, ListNode *n2)
{
    ListNode *tmpNext;
    ListNode *tmpPrev;

    tmpNext = n1->next;
    tmpPrev = n1->prev;
    n1->next = n2->next;
    n1->prev = n2->prev;
    n2->next = tmpNext;
    n2->prev = tmpPrev;
}



int List_bubble_sort(List *list, List_compare compareFunc)
{
    int i;
    int sorted;
    ListNode *node;
    ListNode *next;

    for (i = 0; i < list->length; i++) {
        node = list->first;
        sorted = 1;
        while (node != NULL) {
            next = node->next;
            if (next == NULL) {
                break;
            }

            if (compareFunc(node, next) > 0) {
                List_swap_nodes(node, next);
                sorted = 0;
            } else if (compareFunc(node, next) < 0) {
                List_swap_nodes(next, node);
                sorted = 0;
            }
        }

        if (sorted) {
            break;
        }
    }

    return 0;
}


int List_merge_runs(ListNode *left, ListNode *right, int size)
{

}


int List_merge_sort(List *list)
{
    Darray *array;

    array = List_to_darray(list);
    if (array == NULL) {
        return -1;
    }

    
}