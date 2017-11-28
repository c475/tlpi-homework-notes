#include <sys/sem.h>
#include <fcntl.h>
#include <stdlib.h>
#include "semun.h"


int destroyEventFlag(int flagId)
{
    union semun dummy;

    return semctl(flagId, 0, IPC_RMID, dummy);
}


int setEventFlag(int flagId)
{
    union semun arg;
    
    arg.val = 0;

    return semctl(flagId, 0, SETVAL, arg);
}


int clearEventFlag(int flagId)
{
    union semun arg;

    arg.val = 1;

    return semctl(flagId, 0, SETVAL, arg);
}

// wait for 0
int waitForEventFlag(int flagId)
{
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = 0;
    sops.sem_flg = 0;

    return semop(flagId, &sops, 1);
}


int createEventFlag(const char *key)
{
    int flagId;
    key_t flagKey;

    flagKey = key == NULL ? IPC_PRIVATE : ftok(key, 'x');
    flagId = semget(flagKey, 1, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);
    if (flagId == -1) {
        return -1;
    }

    if (clearEventFlag(flagId) == -1) {
        return -1;
    }

    return flagId;
}

int getEventFlag(const char *key)
{
    int flagId;
    key_t flagKey;

    flagKey = key == NULL ? IPC_PRIVATE : ftok(key, 'x');
    flagId = semget(flagKey, 1, 0);
    if (flagId == -1) {
        return -1;
    }

    return flagId;
}


int getFlagState(int flagId)
{
    union semun arg;
    unsigned short arr[1];

    arg.array = arr;
    if (semctl(flagId, 0, GETALL, arg) == -1) {
        return -1;
    }

    return arg.array[0];
}
