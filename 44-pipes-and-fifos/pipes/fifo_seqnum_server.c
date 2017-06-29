#include <signal.h>
#include "fifo_seqnum.h"


int main(int argc, char *argv[])
{
    int serverFd;
    int dummyFd; // dummy so you don't get EOF on the server FIFO
    int clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0; // this is our "service"

    // create well-known FIFO and open it for reading

    umask(0); // world writable/readable
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        errExit("mkfifo %s", SERVER_FIFO);
    }

    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1) {
        errExit("open %s", SERVER_FIFO);
    }

    // open a write descriptor to the server fifo so you dont see EOF
    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1) {
        errExit("open %s", SERVER_FIFO);
    }

    // maybe not the best option, but ignore SIGPIPE in the event that the server writes to
    // a client FIFO that does not have a reader (broken pipe situation)
    // SIGPIPE terminates the process by default
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        errExit("signal");
    }

    // run the sevrer indefinitely
    // this is an "iterative server", handling clients 1 by 1
    // it's fast enough that creating a new thread to deal with a client request isn't really needed
    for (;;) {
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading requestl discarding\n");
            continue;
        }

        // open the client FIFO
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)req.pid);

        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1) { // open failed, give up on client
            errMsg("open %s", clientFifo);
            continue;
        }

        // send response and close FIFO
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        }

        if (close(clientFd) == -1) {
            errMsg("close");
        }

        seqNum += req.seqLen; // update the sequence number
    }

    exit(EXIT_SUCCESS);
}
