#include <stddef.h>
#include <mqueue.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"

#define PID_BUFFER_SIZE 64


struct client {
    mqd_t mq;
    pid_t pid;
    char name[NAME_MAX];
};


struct client *clientList = NULL;


void initializeClientList(void)
{
    int fd;
    long pidMax;
    char pidBuffer[PID_BUFFER_SIZE];

    memset(pidBuffer, 0, PID_BUFFER_SIZE);

    fd = open("/proc/sys/kernel/pid_max", O_RDONLY);
    if (fd == -1) {
        errExit("open");
    }

    if (read(fd, pidBuffer, PID_BUFFER_SIZE) == -1) {
        errExit("read");
    }

    pidMax = getLong(pidBuffer, GN_GT_0, "pidMax");

    clientList = calloc(1, sizeof(struct client) * pidMax);
    if (clientList == NULL) {
        errExit("clientList");
    }
}


int main(int argc, char *argv[])
{
    initializeClientList();

    printf("%ld\n", sizeof(clientList));

    free(clientList);

    exit(EXIT_SUCCESS);
}
