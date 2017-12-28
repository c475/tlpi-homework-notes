#include "../../lib/tlpi_hdr.h"
#include <pthread.h>

#define NUM_THREADS 60


struct Workload {
    int start;
    int stop;
    Hashmap *map;
    Wordlist *list;
};


void *threadFunc(void *arg)
{
    int i;
    int *val;
    struct Workload *work;
    char *word;

    work = (struct Workload *)arg;
    val = calloc(1, sizeof(int));
    *val = 400;

    for (i = work->start; i < work->stop; i++) {
        word = Darray_get(work->list->words, i);
        if (word == NULL) {
            errExit("Darray_get");
        }

        Hashmap_set(work->map, word, val);
    }

    free(val);
    free(work);

    return NULL;
}


int traverse(HashmapNode *node)
{
    printf("%s: %d\n", (char *)node->key, *(int *)node->data);
    return 0;
}


int main(int argc, char *argv[])
{
    int i;
    struct Workload *work;
    pthread_t *threads;
    size_t size;
    size_t perThread;
    Wordlist *list;
    Hashmap *map;

    list = Wordlist_create("../../resources/dictionary.txt", '\n');
    if (list == NULL) {
        errExit("Wordlist_create");
    }

    map = Hashmap_create(NULL, NULL, 1);
    if (map == NULL) {
        errExit("Hashmap_create");
    }

    threads = calloc(1, sizeof(pthread_t) * NUM_THREADS);
    if (threads == NULL) {
        errExit("calloc");
    }

    printf("Wordlist created... getting to work now\n");

    size = Darray_count(list->words);
    perThread = size / NUM_THREADS;

    for (i = 0; i < NUM_THREADS; i++) {
        work = calloc(1, sizeof(struct Workload));
        work->map = map;
        work->list = list;
        work->start = perThread * i;
        if (i + 1 == NUM_THREADS) {
            work->stop = size - 1;
        } else {
            work->stop = perThread * (i + 1);
        }

        if (pthread_create(&threads[i], NULL, threadFunc, work) != 0) {
            errExit("pthread_create");
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            errExit("pthread_join");
        }
    }

    printf("Done... cleaning up now\n");

    // Hashmap_traverse(map, traverse);

    Wordlist_free(list);
    Hashmap_destroy(map, 0);
    free(threads);

    exit(EXIT_SUCCESS);
}
