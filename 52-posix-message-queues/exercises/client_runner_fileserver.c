#include <pthread.h>
#include "../../lib/tlpi_hdr.h"

#define NUM_THREADS 100


void *threadFunc(void *arg)
{
    for (;;) {
        // try looping forever in a sec
        system("./52-3-client /etc/services 1> /dev/null");
    }

    pthread_exit(0);
}


int main(int argc, char *argv[])
{
    int i;
    int s;
    pthread_t *threads;

    threads = malloc(sizeof(pthread_t) * NUM_THREADS);
    if (threads == NULL) {
        errExit("malloc");
    }

    for (i = 0; i < NUM_THREADS; i++) {
        s = pthread_create(&threads[i], NULL, threadFunc, NULL);
        if (s != 0) {
            errExit("pthread_create");
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        s = pthread_join(threads[i], NULL);
        if (s != 0) {
            errExit("pthread_join");
        }
    }

    exit(EXIT_SUCCESS);
}
