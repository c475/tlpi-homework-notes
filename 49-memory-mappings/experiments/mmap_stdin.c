#include <sys/mman.h>
#include <stdio.h>
#include "../../lib/tlpi_hdr.h"


#define MEM_SIZE 128


int main(int argc, char *argv[])
{
    char *mem;

    // can't mmap() a tty
    mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, STDIN_FILENO, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    if (munmap(mem, MEM_SIZE) == -1) {
        errExit("munmap");
    }

    exit(EXIT_SUCCESS);
}
