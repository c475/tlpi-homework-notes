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

// it really is the best way to do this unfortunately...
#define WRITE_SEM 0
#define READ_SEM 1


struct memseg {
    int length;
    char buf[BUFFER_SIZE];
};


int reserveRead(int semid)
{
    struct sembuf sops;

    sops.sem_num = READ_SEM;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    return semop(semid, &sops, 1);
}


int releaseWrite(int semid)
{
    struct sembuf sops;

    sops.sem_num = WRITE_SEM;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    return semop(semid, &sops, 1);
}

int main(int argc, char *argv[])
{
    int fd;
    int transfers;
    int totalBytes;
    int semid;
    void *mem;
    key_t semToken;
    struct memseg *seg;

    fd = open(MMAP_FILE, O_RDONLY);
    if (fd == -1) {
        errExit("open");
    }

    mem = mmap(NULL, sizeof(struct memseg), PROT_READ, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    semToken = ftok(SEM_KEY, 'x');
    semid = semget(semToken, 2, 0);
    if (semid == -1) {
        errExit("semget");
    }

    for (transfers = 0, totalBytes = 0;; transfers++) {

        if (reserveRead(semid) == -1) {
            errExit("reserveRead");
        }

        seg = mem;

        if (seg->length == 0) {
            break;
        }

        totalBytes += seg->length;

        if (write(STDOUT_FILENO, seg->buf, seg->length) != seg->length) {
            fatal("partial/failed write");
        }

        if (releaseWrite(semid) == -1) {
            errExit("releaseWrite");
        }
    }

    if (munmap(mem, sizeof(struct memseg)) == -1) {
        errExit("munmap");
    }

    if (releaseWrite(semid) == -1) {
        errExit("releaseWrite");
    }

    exit(EXIT_SUCCESS);
}
