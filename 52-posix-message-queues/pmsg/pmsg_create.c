#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name [octal-perms]\n", progName);
    fprintf(stderr, "    -c            Create queue (O_CREAT)\n");
    fprintf(stderr, "    -m maxmsg     Set maximum # of messages\n");
    fprintf(stderr, "    -s msgsize    Set maximum message size\n");
    fprintf(stderr, "    -x            Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int flags;
    int opt;
    mode_t perms;
    mqd_t mqd;
    struct mq_attr attr;
    struct mq_attr *attrp;

    attrp = NULL;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 2048;
    flags = O_RDWR;

    // parse command line arguments
    while ((opt = getopt(argc, argv, "cm:s:x")) != -1) {
        switch (opt) {
            case 'c':
                flags |= O_CREAT;
                break;

            case 'm':
                attr.mq_maxmsg = getInt(optarg, GN_GT_0, "mq_maxmsg");
                attrp = &attr;
                break;

            case 's':
                attr.mq_msgsize = getInt(optarg, GN_GT_0, "mq_msgsize");
                attrp = &attr;
                break;

            case 'x':
                flags |= O_EXCL;
                break;

            default:
                usageError(argv[0]);
        }
    }

    if (optind >= argc) {
        usageError(argv[0]);
    }

    perms = argc <= optind + 1 ? (S_IRUSR | S_IWUSR) : getInt(argv[optind + 1], GN_BASE_8, "octal-perms");

    mqd = mq_open(argv[optind], flags, perms, attrp);
    if (mqd == (mqd_t)-1) {
        errExit("mq_open");
    }

    exit(EXIT_SUCCESS);
}
