/*
    49-2. Rewrite the programs in Listing 48-2 ( svshm_xfr_writer.c , page 1003) and Listing 48-3
    ( svshm_xfr_reader.c , page 1005) to use a shared memory mapping instead of System V
    shared memory.
*/

#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define SEM_KEY "/tmp/semkey2"
#define MMAP_FILE "/tmp/mmap_tmp_file"
#define BUFFER_SIZE 1024
#define WRITE_SEM 0
#define READ_SEM 1


struct memseg {
    int length;
    char buf[BUFFER_SIZE];
};

// initialize semaphgores to write=available, read=unavailable
static unsigned short ini_sems[2] = {1, 0};


// create and initialize the semaphore set
int getSemaphoreSet(const char *key)
{
    union semun arg;
    key_t token;
    int semid;

    token = ftok(key, 'x');
    semid = semget(token, 2, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (semid != -1) {
        arg.array = ini_sems;
        if (semctl(semid, 0, SETALL, arg) == -1) {
            return -1;
        }
    }

    return semid;
}


int reserveWrite(int semid)
{
    struct sembuf sops;

    sops.sem_num = WRITE_SEM;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    return semop(semid, &sops, 1);
}


int releaseRead(int semid)
{
    struct sembuf sops;

    sops.sem_num = READ_SEM;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    return semop(semid, &sops, 1);
}


// event flags suck
int main(int argc, char *argv[])
{
    int keyfd;
    int fd;
    int semid;
    int transfers;
    int totalBytes;
    void *mem;
    struct memseg seg;
    union semun dummy;
    size_t memSize;

    memSize = sizeof(struct memseg);

    // create the semaphore key file...
    keyfd = open(SEM_KEY, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (keyfd == -1) {
        errExit("open");
    }

    if (close(keyfd) == -1) {
        errExit("close");
    }

    fd = open(MMAP_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd == -1) {
        errExit("open");
    }

    // DON'T FORGET TO MAKE SURE THE FILE IS THE CORRECT SIZE (OR YOU GET A BUS ERROR)
    if (ftruncate(fd, memSize) == -1) {
        errExit("ftrincate");
    }

    mem = mmap(NULL, memSize, PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    if (close(fd) == -1) {
        errExit("close");
    }

    semid = getSemaphoreSet(SEM_KEY);
    if (semid == -1) {
        errExit("semget");
    }

    printf("%ld\n", memSize);

    for (transfers = 0, totalBytes = 0;; transfers++) {

        if (reserveWrite(semid) == -1) {
            errExit("reserveWrite");
        }

        seg.length = read(STDIN_FILENO, seg.buf, BUFFER_SIZE);

        if (seg.length == -1) {
            errExit("read");
        }

        memcpy(mem, &seg, memSize);

        if (msync(mem, memSize, MS_SYNC) == -1) {
            errExit("msync");
        }

        if (seg.length == 0) {
            break;
        }

        totalBytes += seg.length;

        if (releaseRead(semid) == -1) {
            errExit("releaseRead");
        }
    }

    if (munmap(mem, memSize) == -1) {
        errExit("munmap");
    }

    if (reserveWrite(semid) == -1) {
        errExit("reserveWrite");
    }

    if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
        errExit("semctl");
    }

    printf("[%ld] Sent %d bytes in %d transfers\n", (long)getpid(), totalBytes, transfers);

    exit(EXIT_SUCCESS);
}
