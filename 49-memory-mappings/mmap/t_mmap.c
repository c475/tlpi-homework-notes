#include <sys/mman.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define MEM_SIZE 1024


int main(int argc, char *argv[])
{
    char *addr;
    int fd;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s file [new-value]\n", argv[0]);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        errExit("open");
    }

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }

    // no longer need "fd"
    if (close(fd) == -1) {
        errExit("close");
    }

    // secure practice, output at most MEM_SIZE bytes (never seen this one before)
    printf("Current string=%.*s\n", MEM_SIZE, addr);

    if (argc > 2) {
        if (strlen(argv[2]) >= MEM_SIZE) {
            cmdLineErr("'new-value' too large\n");
        }

        memset(addr, 0, MEM_SIZE); // zero out memory

        strncpy(addr, argv[2], MEM_SIZE - 1);
        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            errExit("msync");
        }

        printf("Copied '%s' to shared memory\n", argv[2]);
    }

    exit(EXIT_SUCCESS);
}
