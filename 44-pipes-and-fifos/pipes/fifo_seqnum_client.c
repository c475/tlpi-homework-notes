#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];


// invoked on exit to delete client FIFO
static void removeFifo(void)
{
    unlink(clientFifo);
}


int main(int argc, char *argv[])
{
    int serverFd;
    int clientFd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [seq-len...]\n", argv[0]);
    }

    // create the client FIFO before sending the request
    umask(0);

    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());

    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        errExit("mkfifo %s", clientFifo);
    }

    if (atexit(removeFifo) != 0) {
        errExit("atexit");
    }

    // construct request message, open server FIFO, and send request

    req.pid = getpid();
    req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1) {
        errExit("open %s", SERVER_FIFO);
    }

    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
        fatal("can't write to server");
    }

    // open client fifo, read and display response
    clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1) {
        errExit("open %s", clientFifo);
    }

    if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
        fatal("can't read response from server");
    }

    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}
