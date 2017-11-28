#include <sys/types.h>
#include <sys/sem.h>
#include "semun.h"
#include "binary_sems.h"


Boolean bsUseSemUndo = FALSE;
Boolean bsRetruOnEintr = TRUE;


// initializes semaphore to "available (1)"
int initSemAvailable(int semId, int semNum)
{
    union semun arg;

    arg.val = 1;

    return semctl(semId, semNum, SETVAL, arg);
}


// initializes semaphore to "in use (0)"
int initSemInUse(int semId, int semNum)
{
    union semun arg;

    arg.val = 0;

    return semctl(semId, semNum, SETVAL, arg);
}


// reserve semaphore
int reserveSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flag = bsUseSemUndo ? SEM_UNDO : 0;

    while (semop(semId, &sops, 1) == -1) {
        if (errno != EINTR || !bsRetryOnEintr) {
            return -1;
        }
    }

    return 0;
}


int reserveSemNB(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = (bsUseSemUndo ? SEM_UNDO : 0) | IPC_NOWAIT;

    while (semop(semId, &sops, 1) == -1) {
        if 
    }
}


// release semaphore
int releaseSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;

    return semop(semId, &sops, 1);
}
