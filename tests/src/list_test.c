#include "../../lib/tlpi_hdr.h"


int remove_cb(void *data)
{
    int *val;

    val = (int *)data;

    return *val & 1; // remove all odd numbers
}

int find_cb(void *data, void *extra)
{
    int *val;

    val = (int *)data;

    return *val > 80; // should return 82
}


// make all the evens odds
int foreach_minusone_cb(void *data, void *extra)
{
    int *incr;
    int *val;

    val = (int *)data;
    incr = (int *)extra;

    *val -= *incr;

    return 0;
}


int foreach_print_cb(void *data, void *extra)
{
    int *val;

    val = (int *)data;

    printf("%d\n", *val);

    return 0;
}


int List_blaster_test(List *list)
{
    int *val;
    int i;

    for (i = 0; i < 1000000; i++) {
        val = calloc(1, sizeof(int));
        if (val == NULL) {
            errExit("List_blaster_test");
        }

        *val = i;
        List_append(list, val);
    }

    return 0;
}


int List_remove_when_test(List *list)
{
    List_remove_when(list, remove_cb);

    return 0;
}


int List_find_first_test(List *list)
{
    int *x;

    x = List_find_first(list, find_cb, NULL);

    printf("x: %d\n", *x);

    if (*x != 82) {
        errExit("List_find_first_test");
    }

    return 0;
}


int List_foreach_test(List *list)
{
    int x;

    x = 1;

    List_foreach(list, foreach_minusone_cb, &x);
    List_foreach(list, foreach_print_cb, NULL);

    return 0;
}


int List_print_and_destroy_test(List *list)
{
    printf("Length: %ld\n", List_length(list));
    printf("Size: %ld\n", List_size(list));
    List_destroy(list);

    return 0;
}


int List_copy_test(List *list)
{
    
}


int List_thread_blaster_test


int main(int argc, char *argv[])
{
    List *list;

    list = List_create(sizeof(int));
    if (list == NULL) {
        errExit("List_create");
    }

    List_blaster_test(list);
    List_remove_when_test(list);
    List_find_first_test(list);
    List_foreach_test(list);
    List_thread_blaster_test(list);

    List_print_and_destroy_test(list);

    exit(EXIT_SUCCESS);
}
