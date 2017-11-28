#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define SHM_KEY  0xBEEF // key for shared memory segment
#define SEM_KEY "/tmp/OK" // key for semaphore set

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#ifndef BUF_SIZE
#define BUF_SIZE 1024 // allow cc -D to override definition
#endif

struct shmseg {
    int cnt; // number of bytes used in "buf"
    char buf[BUF_SIZE]; // data being transferred
};


int main(int argc, char *argv[])
{
    int flagId;
    int shmid;
    int xfrs;
    int bytes;
    struct shmseg *shmp;

    // get IDS for semaphore set and shared memory created by writer
    flagId = getEventFlag(SEM_KEY);
    if (flagId == -1) {
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
        if (waitForEventFlag(flagId) == -1) {
            errExit("waitForEventFlag");
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
        if (clearEventFlag(flagId) == -1) {
            errExit("setEventFlag");
        }
    }

    if (shmdt(shmp) == -1) {
        errExit("shmdt");
    }

    // give writer one more turn, so it can clean up
    if (setEventFlag(flagId) == -1) {
        errExit("releaseSem");
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
