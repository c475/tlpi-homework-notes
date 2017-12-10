#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int fd;
    void *mem;
    long pageSize;
    char *chunk1;
    char *chunk2;
    char *chunk3;

    fd = open("/tmp/nonlinear", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open");
    }

    // make sure the file starts off clean
    if (ftruncate(fd, 0) == -1) {
        errExit("ftruncate");
    }

    pageSize = sysconf(_SC_PAGE_SIZE);

    if (ftruncate(fd, pageSize * 3) == -1) {
        errExit("ftruncate");
    }

    mem = mmap(NULL, pageSize * 3, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    chunk1 = malloc(pageSize);
    if (chunk1 == NULL) {
        errExit("malloc");
    }

    chunk2 = malloc(pageSize);
    if (chunk1 == NULL) {
        errExit("malloc");
    }

    chunk3 = malloc(pageSize);
    if (chunk1 == NULL) {
        errExit("malloc");
    }

    memset(chunk1, 'A', pageSize);
    memset(chunk2, 'B', pageSize);
    memset(chunk3, 'C', pageSize);

    memcpy(mem, chunk1, pageSize);
    memcpy(mem + pageSize, chunk2, pageSize);
    memcpy(mem + pageSize * 2, chunk3, pageSize);

    if (msync(mem, pageSize * 3, MS_SYNC) == -1) {
        errExit("msync");
    }

    // remap page 2 to 0
    if (remap_file_pages(mem, pageSize, 0, 2, 0) == -1) {
        errExit("remap_file_pages");
    }
    // remap page 0 to page 2
    if (remap_file_pages(mem + 2 * pageSize, pageSize, 0, 0, 0) == -1) {
        errExit("remap_file_pages");
    }

    // it doesnt actually sync to the file in the new order, this call is useless
    if (msync(mem, pageSize * 3, MS_SYNC) == -1) {
        errExit("msync");
    }

    ((char *)mem)[pageSize * 3 - 1] = '\0';

    // you can see it prints the pages in "reverse" order
    printf("%s\n", (char *)mem);

    exit(EXIT_SUCCESS);
}
