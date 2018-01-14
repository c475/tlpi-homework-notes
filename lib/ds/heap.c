#include "heap.h"


void *Heap_root(Heap *heap)
{
    assert(heap);

    return Heap_get(heap, 0);
}


static inline void Heap_set(Heap *heap, int index, void *value)
{
    Darray_set(heap->contents, index, value);
}


// sift the last element up to where it belongs
void Heap_sift_up(Heap *heap)
{
    int i;
    int p;
    int cmp;
    void *child;
    void *parent;

    assert(heap);

    if (Heap_count(heap) <= 1) {
        return;
    }

    for (i = Heap_end(heap) - 1; i > 0; i = (i - 1) / 2) {
        // Darray_debug(heap->contents);

        p = (i - 1) / 2;
        // printf("i: %d, p: %d\n", i, p);

        child = Heap_get(heap, i);
        parent = Heap_get(heap, p);
        cmp = heap->cmp(parent, child);

        // max-heap
        // parent is less than child, so move child up
        if (cmp > 0 && heap->type == 0) {
            // printf("swapping %d with %d\n", *(int *)parent, *(int *)child);
            Heap_set(heap, i, parent);
            Heap_set(heap, p, child);

        // min-heap
        // parent is greater than child, so move child up
        } else if (cmp <= 0 && heap->type != 0) {
            // printf("swapping\n");
            Heap_set(heap, i, parent);
            Heap_set(heap, p, child);

        } else {
            break;
        }
    }
}


// sift the first element down to where it belongs
void Heap_sift_down(Heap *heap)
{
    int i;
    int p;
    int cmp;
    int end;
    void *child1;
    void *child2;
    void *greaterChild;
    void *parent;

    assert(heap);

    if (Heap_count(heap) <= 1) {
        return;
    }

    end = Heap_end(heap);

    i = 0;

    while (i < end) {

        p = i * 2 + 1;

        parent = Heap_get(heap, i);

        // parent has no children, end of the line
        if (p >= end) {
            break;

        // parent has 1 child
        } else if (p + 1 >= end) {
            greaterChild = Heap_get(heap, p);

        // parent has 2 children
        // figure out which one needs to be compared to the parent
        } else {
            child1 = Heap_get(heap, p);
            child2 = Heap_get(heap, p + 1);
            cmp = heap->cmp(child1, child2);

            if (heap->type == 0) {
                if (cmp > 0) {
                    greaterChild = child2;
                    p = p + 1;
                } else {
                    greaterChild = child1;
                }
            } else {
                if (cmp > 0) {
                    greaterChild = child2;
                    p = p + 1;
                } else {
                    greaterChild = child1;
                }
            }
        }

        cmp = heap->cmp(parent, greaterChild);

        // max-heap
        // parent is less than child
        if (cmp > 0 && heap->type == 0) {
            Heap_set(heap, i, greaterChild);
            Heap_set(heap, p, parent);

        // min-heap
        // parent is greater than child
        } else if (cmp <= 0 && heap->type != 0) {
            Heap_set(heap, i, greaterChild);
            Heap_set(heap, p, parent);

        // element is where it needs to be
        } else {
            break;
        }

        i = p;
    }
}


void Heap_push(Heap *heap, void *value)
{
    assert(heap);
    assert(value);

    Darray_push(heap->contents, value);

    Heap_sift_up(heap);
}


void *Heap_pop(Heap *heap)
{
    void *oldRoot;
    void *last;

    assert(heap);

    oldRoot = Darray_get(heap->contents, 0);
    last = Darray_pop(heap->contents);
    Darray_set(heap->contents, 0, last);

    Heap_sift_down(heap);

    return oldRoot;
}


void *Heap_pop_replace(Heap *heap, void *value)
{
    void *root;

    assert(heap);

    root = Darray_get(heap->contents, 0);

    Darray_set(heap->contents, 0, value);

    Heap_sift_down(heap);

    return root;
}


Heap *Heap_create(size_t elementSize, int type, Heap_compare cmp)
{
    Heap *heap;
    pthread_mutex_t *mtx;

    assert(cmp);
    assert(elementSize > 0);

    heap = calloc(1, sizeof(Heap));
    if (heap == NULL) {
        return NULL;
    }

    heap->contents = Darray_create(elementSize, HEAP_INITIAL_ARRAY_SIZE);
    if (heap->contents == NULL) {
        free(heap);
        return NULL;
    }

    mtx = calloc(1, sizeof(pthread_mutex_t));
    if (mtx == NULL) {
        Darray_destroy(heap->contents);
        free(heap);
        return NULL;
    }

    if (pthread_mutex_init(mtx, NULL) == -1) {
        Darray_destroy(heap->contents);
        free(heap);
        free(mtx);
        return NULL;
    }

    heap->mtx = mtx;
    heap->type = type;
    heap->cmp = cmp;
    heap->elementSize = elementSize;

    return heap;
}


// free the underlying data of the heap, but not the heap itself
void Heap_clear(Heap *heap)
{
    assert(heap);

    Darray_clear(heap->contents);
}


// frees the underlying data AND the heap 
void Heap_destroy(Heap *heap)
{
    assert(heap);

    Darray_clear_destroy(heap->contents);

    free(heap->mtx);
    free(heap);
}


// frees the heap, leaving the underlying data in tact. Internal use
static void Heap_free(Heap *heap)
{
    assert(heap);

    Darray_destroy(heap->contents);

    free(heap->mtx);
    free(heap);
}


// yikes
Heap *Heap_from_array(void **array, int size, size_t elementSize, int type, Heap_compare cmp)
{
    int i;
    Heap *heap;

    assert(array);
    assert(cmp);
    assert(size);

    heap = Heap_create(elementSize, type, cmp);
    if (heap == NULL) {
        return NULL;
    }

    for (i = 0; i < size; i++) {
        Heap_push(heap, array[i]);
    }

    return heap;
}


Heap *Heap_from_list(List *list, int type, Heap_compare cmp)
{
    Heap *heap;
    ListNode *node;

    assert(list);

    heap = Heap_create(list->elementSize, type, cmp);
    if (heap == NULL) {
        return NULL;
    }

    node = list->first;
    while (node != NULL) {
        Heap_push(heap, node->value);
        node = node->next;
    }

    return heap;
}


Heap *Heap_from_darray(Darray *array, int type, Heap_compare cmp)
{
    int i;
    Heap *heap;

    assert(array);

    heap = Heap_create(array->elementSize, type, cmp);
    if (heap == NULL) {
        return NULL;
    }

    for (i = 0; i < Darray_count(array); i++) {
        Heap_push(heap, Darray_get(array, i));
    }

    return heap;
}


// there is probably some genius heap merging algorithm, look it up later
// appropriates the left->cmp for use in the new heap
// and the left->type
Heap *Heap_merge(Heap *left, Heap *right)
{
    int i;
    Heap *newHeap;

    assert(left);
    assert(right);
    assert(left->elementSize == right->elementSize);

    newHeap = Heap_create(left->elementSize, left->type, left->cmp);
    if (newHeap == NULL) {
        return NULL;
    }

    for (i = 0; i < Heap_count(left); i++) {
        Heap_push(newHeap, Darray_get(left->contents, i));
    }

    for (i = 0; i < Heap_count(right); i++) {
        Heap_push(newHeap, Darray_get(right->contents, i));
    }

    return newHeap;
}


Heap *Heap_meld(Heap *left, Heap *right)
{
    Heap *newHeap;

    newHeap = Heap_merge(left, right);
    if (newHeap == NULL) {
        return NULL;
    }

    Heap_free(left);
    Heap_free(right);

    return newHeap;
}


void Heap_debug(Heap *heap)
{
    Darray_debug(heap->contents);
}
