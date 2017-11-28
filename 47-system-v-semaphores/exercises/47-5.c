#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


union semun { // used in calls to semctl()
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};


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


int main(int argc, char *argv[])
{
    int flagId;
    pid_t childPid;

    // create a pre-cleared event flag
    flagId = createEventFlag(NULL);
    if (flagId == -1) {
        errExit("createEventFlag");
    }

    switch (childPid = fork()) {
        case -1:
            errExit("fork");

        case 0:
            printf("[%ld] Child doing work\n", (long)getpid());

            sleep(3);

            printf("[%ld] Child about to set event flag and exit\n", (long)getpid());

            if (setEventFlag(flagId) == -1) {
                errExit("setEventFlag");
            }

            _exit(EXIT_SUCCESS);

        default:
            printf("[%ld] Parent waiting for event flag from child, flag state: %d\n", (long)getpid(), getFlagState(flagId));

            if (waitForEventFlag(flagId) == -1) {
                errExit("waitForEventFlag");
            }

            printf("[%ld] Parent got event flag, flag state: %d\n", (long)getpid(), getFlagState(flagId));
    }

    // cleanup
    if (destroyEventFlag(flagId) == -1) {
        errExit("destroyEventFlag");
    }

    exit(EXIT_SUCCESS);
}
