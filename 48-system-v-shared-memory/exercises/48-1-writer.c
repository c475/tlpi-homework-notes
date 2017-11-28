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
    int bytes;
    int xfrs;
    int flagState;
    struct shmseg *shmp;

    flagId = createEventFlag(SEM_KEY);
    if (flagId == -1) {
        errExit("semget");
    }

    shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1) {
        errExit("shmget");
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) {
        errExit("shmat");
    }

    // transfer blocks of data from stdin to shared memory
    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {

        // read from stdin
        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            errExit("read");
        }

        if (setEventFlag(flagId) == -1) {
            errExit("setEventFlag");
        }

        // check EOF. Test this after giving the reader a chance to see shmp->cnt == 0
        if (shmp->cnt == 0) {
            break;
        }

        // think you actually have to do this as a guarantee against race conditions when using event flags
        for (;;) {
            flagState = getFlagState(flagId);
            if (flagState == -1) {
                errExit("getFlagState");
            }

            // free to continue
            if (flagState == 0) {
                break;
            }
        }
    }

    // wait until reader gives the writer one more turn at the semaphore, which lets it know that the reader is done
    if (waitForEventFlag(flagId) == -1) {
        errExit("waitForEventFLag");
    }

    if (destroyEventFlag(flagId) == -1) {
        errExit("semctl");
    }

    if (shmdt(shmp) == -1) {
        errExit("shmdt");
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        errExit("shmctl");
    }

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
