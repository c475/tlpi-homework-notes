#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    unsigned int cpu;
    unsigned int node;
    int i;

    for (i = 0; i < 100000; i++) {
        if (syscall(SYS_getcpu, &cpu, &node, NULL) == -1) {
            fprintf(stderr, "getcpu() failed\n");
            exit(EXIT_SUCCESS);
        }

        printf("cup: %d, node: %d\n", cpu, node);

        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
