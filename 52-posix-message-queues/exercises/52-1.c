#include <mqueue.h>
#include <fcntl.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] name\n", progName);
    fprintf(stderr, "    -n        Use O_NONBLOCK\n");
    fprintf(stderr, "    -t        Timeout (seconds)\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int flags;
    int opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;
    struct timespec ts;
    struct timespec *tsp;

    flags = O_RDONLY;

    tsp = NULL;

    while ((opt = getopt(argc, argv, "t:n")) != -1) {
        switch (opt) {
            case 'n':
                flags |= O_NONBLOCK;
                break;

            case 't':
                if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                    errExit("clock_gettime");
                }

                ts.tv_sec += getInt(optarg, GN_NONNEG, "timeout");
                tsp = &ts;
                break;

            default:
                usageError(argv[0]);
        }
    }

    if (optind >= argc) {
        usageError(argv[0]);
    }

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t)-1) {
        errExit("mq_open");
    }

    if (mq_getattr(mqd, &attr) == -1) {
        errExit("mq_getattr");
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        errExit("malloc");
    }

    numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, tsp);
    if (numRead == -1) {
        errExit("mq_receive");
    }

    printf("Read %ld butes; priority = %u\n", (long)numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1) {
        errExit("write");
    }

    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}
