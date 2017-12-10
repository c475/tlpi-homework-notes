#define _GNU_SOURCE
#include <sys/mman.h>
#include "../../lib/tlpi_hdr.h"


// display residency of pages in range [addr ... (addr + length - 1)]
static void displayMincore(char *addr, size_t length)
{
    unsigned char *vec;
    long pageSize;
    long numPages;
    long j;

    pageSize = sysconf(_SC_PAGESIZE);

    numPages = (length + pageSize - 1) / pageSize;
    vec = malloc(numPages);
    if (vec == NULL) {
        errExit("malloc");
    }

    if (mincore(addr, length, vec) == -1) {
        errExit("mincore");
    }

    for (j = 0; j < numPages; j++) {
        if (j % 64 == 0) {
            printf("%s%10p: ", j == 0 ? "" : "\n", addr + (j * pageSize));
        }

        printf("%c", (vec[j] & 1) ? '*' : '.');
    }

    printf("\n");

    free(vec);
}


int main(int argc, char *argv[])
{
    char *addr;
    size_t len;
    size_t lockLen;
    long pageSize;
    long stepSize;
    long j;

    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s num-page lock-page-step lock-page-len\n", argv[0]);
    }

    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1) {
        errExit("sysconf(_SC_PAGESIZE)\n");
    }

    len = getInt(argv[1], GN_GT_0, "num-pages") * pageSize;
    stepSize = getInt(argv[2], GN_GT_0, "lock-page-step") * pageSize;
    lockLen = getInt(argv[3], GN_GT_0, "lock-page-len") * pageSize;

    addr = mmap(NULL, len, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }

    printf("Allocated %ld (%#lx) bytes starting at %p\n", (long)len, (unsigned long)len, addr);

    printf("Before mlock:\n");

    displayMincore(addr, len);

    // lock pages specified by command line argumens into memory
    for (j = 0; j + lockLen <= len; j += stepSize) {
        if (mlock(addr + j, lockLen) == -1) {
            errExit("mlock");
        }
    }

    printf("After mlock:\n");

    displayMincore(addr, len);

    exit(EXIT_SUCCESS);
}
