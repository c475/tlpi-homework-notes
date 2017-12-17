#include <sys/mman.h>
#include <sys/resource.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    struct rlimit rl;
    void *mem;
    void *nomem;
    int pageSize;

    if (getrlimit(RLIMIT_MEMLOCK, &rl) == -1) {
        errExit("getrlimit");
    }

    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1) {
        errExit("sysconf(_SC_PAGESIZE)");
    }

    printf("RLIMIT_MEMLOCK:\n\tsoft: %ld\n\thard: %ld\n", rl.rlim_cur, rl.rlim_max);
    printf("Making a memory map then locking it\n");

    mem = mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    if (mlock(mem, pageSize) == -1) {
        errExit("mlock");
    }

    printf("Success. Now, going to reduce the RLIMIT_MEMLOCK to 0, then try that again.\n");

    rl.rlim_cur = 0;
    rl.rlim_max = 0;

    if (setrlimit(RLIMIT_MEMLOCK, &rl) == -1) {
        errExit("setrlimit");
    }

    if (getrlimit(RLIMIT_MEMLOCK, &rl) == -1) {
        errExit("getrlimit");
    }

    printf("RLIMIT_MEMLOCK:\n\tsoft: %ld\n\thard: %ld\n", rl.rlim_cur, rl.rlim_max);

    nomem = mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (nomem == MAP_FAILED) {
        errExit("mmap");
    }

    if (mlock(nomem, pageSize) == -1) {
        printf("mlock() failed as expected, exiting\n");
        exit(EXIT_SUCCESS);
    }

    printf("mlock() didn't fail for some reason!!!\n");
    exit(EXIT_FAILURE);
}
