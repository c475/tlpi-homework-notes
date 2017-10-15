#ifndef SEMUN_H
#define SEMUN_H // lol...

#include <sys/types.h> // for portability
#include <sys/sem.h>

union semun { // used in calls to semctl()
    int val;
    struct demid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};

#endif
