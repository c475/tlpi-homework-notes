#define _GNU_SOURCE
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    char *mem;
    for (;;) {
        mem = malloc(1<<30);
        if (mem == NULL) {
            printf("OOM\n");
            getchar();
        }

        if (mlock(mem, 1<<30) == -1) {
            errExit("mlock");
        }
    }

    exit(EXIT_SUCCESS);
}
