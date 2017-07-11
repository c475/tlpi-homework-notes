#include <signal.h>
#include "fifo_seqnum.h"

#define BACKUP_PATH "backup.seqnum"


void handler(int sig)
{
    if (unlink(SERVER_FIFO) == -1) {
        errExit("unlink(SERVER_FIFO)");
    }
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
    int backupFd;
    int serverFd;
    int dummyFd; // dummy so you don't get EOF on the server FIFO
    int clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    struct sigaction sa;
    int clientError;
    int seqNum = 0; // this is our "service"

    // set handlers for SIGINT, SIGTERM
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errExit("sigaction (SIGINT)");
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        errExit("sigaction (SIGTERM)");
    }

    // create well-known FIFO and open it for reading

    umask(0); // world writable/readable
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        errExit("mkfifo %s", SERVER_FIFO);
    }

    // have to make sure disk writes are synchronous
    // because if the server is being hit hard the write() call may not 
    // write to disk by the time write() is called again
    backupFd = open(BACKUP_PATH, O_RDWR | O_SYNC);
    if (backupFd == -1) {
        if (errno == ENOENT) {
            backupFd = open(BACKUP_PATH, O_RDWR | O_SYNC | O_CREAT);
            if (backupFd == -1) {
                errExit("open (backup)");
            }

            if (write(backupFd, &seqNum, sizeof(seqNum)) != sizeof(seqNum)) {
                errExit("write (backup)");
            }
        } else {
            errExit("open (backup)");
        }
    } else {
        if (read(backupFd, &seqNum, sizeof(seqNum)) != sizeof(seqNum)) {
            errExit("read (backup)");
        }
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
        clientError = 0;

        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading requestl discarding\n");
            continue;
        }

        // open the client FIFO
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)req.pid);

        // open the client FIFO in Non-blocking mode.
        // the client is modified to create the client FIFO first thing (non-blocking as well)
        // then immediately switches off the non-blocking flag, so the FIFO is "guaranteed" to exist.
        // If someone wants to mess with the client and get rid of this functionality
        // then it doesnt matter, the client just won't work 100% of the time
        clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK);
        if (clientFd == -1) { // open failed, give up on client
            errMsg("open %s", clientFifo);
            continue;
        }

        // send response and close FIFO
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
            clientError = 1;
        }

        if (close(clientFd) == -1) {
            errMsg("close");
            clientError = 1;
        }

        if (clientError == 0) {
            seqNum += req.seqLen; // update the sequence number

            // backup the seqNum
            if (write(backupFd, &seqNum, sizeof(seqNum)) != sizeof(seqNum)) {
                // server goes down...
                break;
            }

            // seek to beginning of file
            if (lseek(backupFd, 0, SEEK_SET) == -1) {
                // server goes down...
                break;
            }
        }
    }

    close(dummyFd);
    close(serverFd);
    close(backupFd);

    exit(EXIT_FAILURE);
}
