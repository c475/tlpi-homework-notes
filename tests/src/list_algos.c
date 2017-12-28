#include <assert.h>
#include "../../lib/tlpi_hdr.h"


int is_sorted(List *lsit)
{
    LIST_FOREACH(words, first, next, cur) {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
            printf("Not sorted: %s > %s\n", cur->next->value, cur->value);
            return 0;
        }    
    }

    return 1;
}


int test_bubble_sort(List *list)
{
    List *empty;
    int rc;

    rc = List_bubble_sort(list, (List_compare)strcmp);
    assert(rc == 0);
    assert(is_sorted(list) == 1);

    rc = List_bubble_sort(list, (List_compare)strcmp);
    assert(rc == 0);
    assert(is_sorted(list) == 1);

    empty = List_create(sizeof(char *));
    rc = List_bubble_sort(empty);
    assert(rc == 0);
    assert(is_sorted(words) == 1); // en empty list should already be "sorted"

    fprintf(stderr, "Bubble sort checks out\n");

    List_clear_destroy(empty);
    List_reverse(list); // make the list unsorted again (in the worst case possible)

    return 0;
}


int test_merge_sort(List *list)
{
    int rc;

    rc = List_merge_sort(words, (List_compare)strcmp);
    assert(rc == 0);
    assert(is_sorted(list) == 1);

    // should work again
    rc = List_merge_sort(words, (List_compare)strcmp);
    assert(rc == 0);
    assert(is_sorted(list) == 1);

    fprintf(stderr, "Merge sort checks out\n");

    List_reverse(list);

    return 0;
}


int main(int argc, char *argv[])
{
    List *words;

    words = Wordlist_create("../../resources/dictionary.txt", '\n');
    if (words == NULL) {
        errExit("Wordlist_create");
    }

    test_bubble_sort(words);
    test_merge_sort(words);

    exit(EXIT_SUCCESS);
}
