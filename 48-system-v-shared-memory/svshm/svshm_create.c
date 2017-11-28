#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int shmid;
    int size;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s size\n", argv[0]);
    }

    size = getLong(argv[1], 0, "size");

    shmid = shmget(IPC_PRIVATE, size, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (shmid == -1) {
        errExit("shmget");
    }

    printf("shmid: %d, size: %d\n", shmid, size);

    exit(EXIT_SUCCESS);
}
