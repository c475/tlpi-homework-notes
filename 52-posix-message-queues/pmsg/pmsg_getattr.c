#include <mqueue.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    mqd_t mqd;
    struct mq_attr attr;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s mq-name\n", argv[0]);
    }

    mqd = mq_open(argv[1], O_RDONLY);
    if (mqd == (mqd_t)-1) {
        errExit("mq_open");
    }

    if (mq_getattr(mqd, &attr) == -1) {
        errExit("mq_getattr");
    }

    printf("Maximum # of messages on queue:   %ld\n", attr.mq_maxmsg);
    printf("Maximum message size:             %ld\n", attr.mq_msgsize);
    printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);

    exit(EXIT_SUCCESS);
}
