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
x

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

    pidMax = strtol(pidBuffer, NULL, 10);
    if (errno == ERANGE) {
        errExit("strtol");
    }

    printf("size: %ld\n", sizeof(struct client) * pidMax + 1);

    clientList = calloc(1, sizeof(struct client) * pidMax + 1);
    if (clientList == NULL) {
        errExit("clientList");
    }
}


int main(int argc, char *argv[])
{
    initializeClientList();

    free(clientList);

    exit(EXIT_SUCCESS);
}
