#include "../../lib/tlpi_hdr.h"

#define PUSH_POP_NUM 10


int heap_cmp(void *a, void *b)
{
    int *x;
    int *y;

    x = (int *)a;
    y = (int *)b;

    return *x < *y;
}


void check_valid_max_heap(Heap *heap, int parent)
{
    int count;
    void *p;
    void *c1;
    void *c2;
    int rc;

    count = Heap_count(heap);

    p = Heap_get(heap, parent);

    if (parent * 2 + 1 < count) {
        c1 = Heap_get(heap, parent * 2 + 1);
        rc = heap_cmp(p, c1);
        if (rc > 0) {
            printf("p: %d, c1: %d\n", *(int *)p, *(int *)c1);
        }
        assert(rc <= 0);
        check_valid_max_heap(heap, parent * 2 + 1);
    }

    if (parent * 2 + 2 < count) {
        c2 = Heap_get(heap, parent * 2 + 2);
        rc = heap_cmp(p, c2);
        if (rc > 0) {
            printf("p: %d, c2: %d\n", *(int *)p, *(int *)c2);
        }
        assert(rc <= 0);
        check_valid_max_heap(heap, parent * 2 + 2);
    }
}


void test_heap_push(Heap *heap)
{
    int i;
    int *val;

    for (i = 0; i < PUSH_POP_NUM; i++) {
        val = calloc(1, sizeof(int));
        assert(val);
        *val = i;
        Heap_push(heap, val);
    }

    check_valid_max_heap(heap, 0);
}


void test_heap_pop(Heap *heap)
{
    int i;
    int *val;

    for (i = 0; i < PUSH_POP_NUM; i++) {
        val = Heap_pop(heap);
        printf("val: %d\n", *val);
        free(val);
    }
}


void test_max_heap(void)
{
    Heap *heap;

    heap = Heap_create(sizeof(int), 0, heap_cmp);
    assert(heap);

    test_heap_push(heap);

    Heap_debug(heap);

    test_heap_pop(heap);

    printf("GONNA DESTROY, count: %d\n", Darray_count(heap->contents));

    Heap_destroy(heap);
}


void test_min_heap(void)
{
    Heap *heap;

    heap = Heap_create(sizeof(int), 1, heap_cmp);
    assert(heap);

    // test_heap_push(heap);
    // Darray_debug(heap->contents);
}



int main(int argc, char *argv[])
{
    test_max_heap();

    exit(EXIT_SUCCESS);
}
