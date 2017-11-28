#include "svshm_xfr.h"


int main(int argc, char *argv[])
{
    int semid;
    int shmid;
    int xfrs;
    int bytes;
    struct shmseg *shmp;

    // get IDS for semaphore set and shared memory created by writer
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        errExit("semget");
    }

    shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1) {
        errExit("shmid");
    }

    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *)-1) {
        errExit("shmat");
    }

    // Transfer blocks of data from shared memory to stdout
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        // wait for turn
        if (reserveSem(semid, READ_SEM) == -1) {
            errExit("reserveSem");
        }

        // EOF from writer?
        if (shmp->cnt == 0) {
            break;
        }

        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
            fatal("partial/failed write");
        }

        // give writer a turn
        if (releaseSem(semid, WRITE_SEM) == -1) {
            errExit("releaseSem");
        }
    }

    if (shmdt(shmp) == -1) {
        errExit("shmdt");
    }

    // give writer one more turn, so it can clean up
    if (releaseSem(semid, WRITE_SEM) == -1) {
        errExit("releaseSem");
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
