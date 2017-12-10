#ifndef USE_MAP_ANON
#define _BSD_SOURCE
#endif

#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int *addr; // pointer to shared memory region

#ifndef USE_MAP_ANON // use MAP_ANONYMOUS
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }
#else // map /dev/zero
    int fd;

    fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        errExit("open");
    }

    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }
#endif

    *addr = 1; // initialize the memory

    switch (fork()) {
        case -1:
            errExit("fork");

        case 0:
            printf("Child started, value = %d\n", *addr);
            (*addr)++;

            if (munmap(addr, sizeof(int)) == -1) {
                errExit("munmap");
            }

            exit(EXIT_SUCCESS);

        default:
            if (wait(NULL) == -1) {
                errExit("wait");
            }

            printf("In parent, value = %d\n", *addr);

            if (munmap(addr, sizeof(int)) == -1) {
                errExit("munmap");
            }

            exit(EXIT_SUCCESS);
    }

    exit(EXIT_FAILURE);
}
