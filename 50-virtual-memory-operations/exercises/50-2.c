#include <sys/mman.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int i;
    char *mem;
    int pageSize;

    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1) {
        errExit("sysconf(_SC_PAGESIZE)");
    }

    mem = mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    printf("Filling memory with 'A'\n");

    memset(mem, 'A', pageSize);

    // confirm memory is filled with 'A'
    for (i = 0; i < pageSize; i++) {
        if (mem[i] != 'A') {
            errExit("memset?");
        }
    }

    printf("Confirmed memory is filled with 'A'\n");

    // reinitialize map with madvise()
    if (madvise(mem, pageSize, MADV_DONTNEED) == -1) {
        errExit("madvise");
    }

    // confirm memory is zeroed out
    for (i = 0; i < pageSize; i++) {
        if (mem[i] != 0) {
            errExit("madvise?");
        }
    }

    printf("Confirmed memory is reinitialized after madvise() MADV_DONTNEED\n");

    exit(EXIT_SUCCESS);
}
