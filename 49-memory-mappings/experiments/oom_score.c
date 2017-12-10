#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../../lib/tlpi_hdr.h"


// test proc/PID/oom_score
int main(int argc, char *argv[])
{
    void *mem;
    int len;

    if (argc != 2) {
        usageErr("%s bytes-of-mem\n", argv[0]);
    }

    len = getInt(argv[1], 0, "len");
    mem = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    printf("/proc/%ld/oom_score\n", (long)getpid());
    sleep(20);

    if (munmap(mem, len) == -1) {
        errExit("munmap");
    }

    exit(EXIT_SUCCESS);
}
