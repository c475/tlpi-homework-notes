#include <pthread.h>
#include <unistd.h>
#include "../../lib/tlpi_hdr.h"

#define NUM_THREADS 80
#define CMD_TEMPLATE "./52-2-client %ld 2>> error.txt 1>> output.txt"
#define CMD_LEN (sizeof(CMD_TEMPLATE) + 1)


void *threadFunc(void *arg)
{
    int i;
    char buff[CMD_LEN];

    memset(buff, 0, CMD_LEN);

    for (i = 0; i < 1000000; i++) {
        snprintf(buff, CMD_LEN+1, CMD_TEMPLATE, (long)i);
        system(buff);
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
