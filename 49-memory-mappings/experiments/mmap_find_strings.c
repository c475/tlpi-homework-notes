#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include "../../lib/tlpi_hdr.h"

#define MAP_UNINITIALIZED 0x4000000
#define len 1 << 20

int isPrintable(char c)
{
    return ((c >= 32 && c <= 126) || (c >= 8 && c <= 13));
}


int main(int argc, char *argv[])
{
    int i;
    char arr[len];

    for (i = 0; i < len; i++) {
        if (isPrintable(arr[i])) {
            printf("%c", arr[i]);
        }
    }

    exit(EXIT_SUCCESS);
}