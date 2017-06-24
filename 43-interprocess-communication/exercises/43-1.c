#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


// test the bandwidth of pipe()s
int main(int argc, char *argv[])
{
    int pipefd[2];
    int numBlocks;
    int blockSize;
    char *block;
    pid_t childPid;
    double elapsed;
    struct timespec start;
    struct timespec stop;

    if (argc != 3) {
        usageErr("%s num-blocks block-size\n", argv[0]);
    }

    if (pipe(pipefd) == -1) {
        errExit("pipe");
    }

    numBlocks = getInt(argv[1], GN_NONNEG, 0);
    blockSize = getInt(argv[2], GN_NONNEG, 0);

    block = calloc(blockSize, 1);

    switch (childPid = fork()) {

        case -1:
            errExit("fork");

        case 0:
            close(pipefd[0]);

            while (numBlocks--) {
                if (write(pipefd[1], block, blockSize) != blockSize) {
                    errExit("write");
                }
            }

            close(pipefd[1]);
            _exit(EXIT_SUCCESS);

        default:
            close(pipefd[1]);

            clock_gettime(CLOCK_MONOTONIC, &start);

            while (read(pipefd[0], block, blockSize) == blockSize);

            clock_gettime(CLOCK_MONOTONIC, &stop);

            close(pipefd[0]);
            wait(NULL);
    }

    free(block);

    elapsed = ((double)stop.tv_sec + 1.0e-9 * stop.tv_nsec) - 
        ((double)start.tv_sec + 1.0e-9 * start.tv_nsec);

    printf(
        "Elapsed seconds: %.6f\nBytes-per-sec:   %.2f\n", 
        elapsed, 
        (blockSize * numBlocks) / elapsed
    );

    exit(EXIT_SUCCESS);
}
