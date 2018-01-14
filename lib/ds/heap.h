#ifndef __HEAP_DS_HEADER__
#define __HEAP_DS_HEADER__

#include "list.h"
#include "hashmap.h"
#include "darray.h"

/*

    Generic binary heap implementation

*/

#define HEAP_INITIAL_ARRAY_SIZE 300


typedef int (*Heap_compare)(void *a, void *b);


typedef struct Heap {
    int type; // 0: max heap, 1: min heap
    Darray *contents; // contents of the heap
    Heap_compare cmp; // comparison callback for nodes
    size_t elementSize;
    pthread_mutex_t *mtx;
} Heap;


#define Heap_count(A) (Darray_count((A)->contents))
#define Heap_end(A) (Heap_count((A)))


static inline void *Heap_get(Heap *heap, int index)
{
    assert(heap);

    return Darray_get(heap->contents, index);
}


static inline int Heap_empty(Heap *heap)
{
    assert(heap);

    return Heap_count(heap) == 0;
}


/* Basic operations */
void *Heap_root(Heap *heap); // return the value of the root node
void Heap_push(Heap *heap, void *value); // insert value into the heap
void *Heap_pop(Heap *heap); // pop the max (or min) element of the heap
void Heap_delete_root(Heap *heap); // remove the max (or min) element of the heap
void *Heap_pop_replace(Heap *heap, void *value); // pop the root node, and replace it with value. more efficient than pop then push because the tree only balances once, not twice.


/* Creation / deletion */
Heap *Heap_create(size_t elementSize, int type, Heap_compare cmp); // 1: min heap, 0: max heap
void Heap_clear(Heap *heap);
void Heap_destroy(Heap *heap);
Heap *Heap_from_array(void **array, int size, size_t elementSize, int type, Heap_compare cmp); // size: size of buffer
Heap *Heap_from_list(List *list, int type, Heap_compare cmp);
Heap *Heap_from_darray(Darray *array, int type, Heap_compare cmp);
Heap *Heap_merge(Heap *left, Heap *right); // merge the left and right heaps, preserving the originals
Heap *Heap_meld(Heap *left, Heap *right); // merge and destroy left and right heaps, returning the new heap


/* Internal */
void Heap_sift(Heap *heap); // sift the last element up the tree until it reaches its destination
void Heap_sift_down(Heap *heap);

void Heap_debug(Heap *heap);

#endif
