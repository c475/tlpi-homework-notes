#include <pthread.h>
#include "../../lib/tlpi_hdr.h"

#define NUM_THREADS 20


static struct List *list = NULL;


void *threadFunc(void *arg)
{
    int i;

    if (list != NULL) {
        for (i = 0; i < 10000; i++) {
            listAppend(list, &i);
            listPrepend(list, &i);
        }
    }

    return NULL;
}


int foreachPrint(void *arg)
{
    int *i;

    i = (int *)arg;

    printf("%d\n", *i);

    return 0;
}


int foreachMultTwo(void *arg)
{
    int *i;

    i = (int *)arg;

    *i *= 2;

    return 0;
}

int removeDivTwo(void *arg)
{
    int *i;

    i = (int *)arg;

    return *i % 2 ? 0 : 1;
}


int threadIt(int argc, char *argv[])
{
    int i;
    pthread_t *threads;
    // struct List *copy;
    // struct List *copy2;
    // struct List *copy3;

    threads = calloc(NUM_THREADS, sizeof(pthread_t));
    if (threads == NULL) {
        errExit("calloc");
    }

    // list of ints
    list = listCreate(sizeof(int));
    if (list == NULL) {
        errExit("listCreate");
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, threadFunc, NULL) != 0) {
            errExit("pthread_create");
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            errExit("pthread_join");
        }
    }

    // copy = listCopy(list);
    // copy2 = listCopy(copy);
    // copy3 = listCopy(copy2);

    // printf("length: %ld\n", (long)listLength(list));
    // printf("length c1: %ld\n", (long)listLength(copy));
    // printf("length c2: %ld\n", (long)listLength(copy2));
    // printf("length c3: %ld\n", (long)listLength(copy3));

    // listDestroy(copy3);
    // listDestroy(copy2);
    // listDestroy(copy);

    // listForeach(list, foreachPrint);
    // listForeach(list, foreachMultTwo);
    // listForeach(list, foreachPrint);

    listRemoveWhen(list, removeDivTwo);
    listForeach(list, foreachPrint);

    listDestroy(list);

    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
    // int i;

    // struct List *list;
    // struct List *copy1;


    // list = listCreate(sizeof(int));
    // if (list == NULL) {
    //     errExit("listCreate");
    // }

    // for (i = 0; i < 1000000; i++) {
    //     listAppend(list, &i);
    //     listPrepend(list, &i);
    // }

    // printf("length: %ld\n", (long)listLength(list));

    // copy1 = listCopy(list);

    // printf("Made copy\n");

    // listDestroy(list);
    // listDestroy(copy1);

    threadIt(argc, argv);

    exit(EXIT_SUCCESS);
}
