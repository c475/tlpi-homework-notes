#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include "semun.h"
#include "../../lib/tlpi_hdr.h"


// monitor a semaphore
int main(int argc, char *argv[])
{
    struct semid_ds ds;
    union semun arg;
    union semun dummy;

    int semid;
    int j;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s semid\n", argv[0]);
    }

    semid = getInt(argv[1], 0, "semid");

    // get info about the semaphore set, the index argument is unused (set to 0)
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1) {
        errExit("semctl");
    }

    printf("Semaphore changed: %s", ctime(&ds.sem_ctime));
    printf("Last semop(): %s\n", ctime(&ds.sem_otime));

    // display per-semaphore information
    arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
    if (arg.array == NULL) {
        errExit("calloc");
    }

    if (semctl(semid, 0, GETALL, arg) == -1) {
        errExit("semctl");
    }

    printf("Sem #  Value  SEMPID  SEMNCNT  SEMZNCT\n");

    for (j = 0; j < ds.sem_nsems; j++) {
        printf(
            "%3d   %5d   %5d  %5d    %5d\n", 
            j, 
            arg.array[j], 
            semctl(semid, j, GETPID, dummy), 
            semctl(semid, j, GETNCNT, dummy), 
            semctl(semid, j, GETZCNT, dummy)
        );
    }

    exit(EXIT_SUCCESS);
}
