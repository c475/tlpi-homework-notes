#include <mqueue.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] name msg [prio]\n", progName);
    fprintf(stderr, "    -n    use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int flags;
    int opt;
    mqd_t mqd;
    unsigned int prio;

    flags = O_WRONLY;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
            case 'n':
                flags |= O_NONBLOCK;
                break;

            default:
                usageError(argv[0]);
        }
    }

    if (optind + 1 >= argc) {
        usageError(argv[0]);
    }

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t)-1) {
        errExit("mq_open");
    }

    prio = argc > optind + 2 ? getInt(argv[optind + 2], GN_NONNEG, "prio") : 0;

    if (mq_send(mqd, argv[optind + 1], strlen(argv[optind + 1]), prio) == -1) {
        errExit("mq_send");
    }

    exit(EXIT_SUCCESS);
}
