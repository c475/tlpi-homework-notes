#include <fcntl.h>
#include "writer_reader_header.c"
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int fd;
    int semid;
    int shmid;
    int bytes;
    int xfrs;
    struct shmseg *shmp;
    union semun dummy;

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
        errExit("semget");
    }

    if (initSemAvailable(semid, WRITE_SEM) == -1) {
        errExit("initSemAvailable");
    }

    if (initSemInUse(semid, READ_SEM) == -1) {
        errExit("initSemInUse");
    }

    shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1) {
        errExit("shmget");
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *)-1) {
        errExit("shmat");
    }

    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        errExit("open");
    }

    // transfer blocks of data from stdin to shared memory
    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {

        // wait for our turn
        if (reserveSem(semid, WRITE_SEM) == -1) {
            errExit("reserveSem");
        }

        // read from stdin
        shmp->cnt = read(fd, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
            errExit("read");
        }

        if (releaseSem(semid, READ_SEM) == -1) {
            errExit("releaseSem");
        }

        // check EOF. Test this after giving the reader a chance to see shmp->cnt == 0
        if (shmp->cnt == 0) {
            break;
        }
    }

    // wait until reader gives the writer one more turn at the semaphore, which lets it know that the reader is done
    if (reserveSem(semid, WRITE_SEM) == -1) {
        errExit("reserveSem");
    }

    if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
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
