#include <sys/types.h>
#include <sys/shm.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int i;
    int shmid;
    long addr;
    char *endptr;
    void *memaddr;

    if (argc < 2 || strcmp(argv[1], "--help") == -1) {
        usageErr("%s shmid:addr...\n\tplease provide addr in hex\n", argv[0]);
    }

    printf("[%ld] SHMLBA: %d (0x%08x)\n", (long)getpid(), SHMLBA, SHMLBA);

    for (i = 1; i < argc; i++) {
        errno = 0;
        shmid = strtol(argv[i], &endptr, 10);
        if (errno != 0) {
            errExit("strtol");
        }

        errno = 0;
        addr = strtol(endptr, NULL, 16);
        if (errno != 0) {
            errExit("strtol");
        }

        memaddr = shmat(shmid, addr == 0 ? NULL : (void *)addr, 0);
        if (memaddr == (void *)-1) {
            errExit("shmat");
        }

        printf("[%ld] shmid %d is attached at %p\n", (long)getpid(), shmid, memaddr);
    }

    exit(EXIT_SUCCESS);
}
