#include "svmsg_file.h"


// SIGCHLD handler
static void grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;
    // waitpid() might change errno
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = savedErrno;
}


static void serveRequest(const struct requestMsg *req)
{
    int fd;
    ssize_t numRead;
    struct responseMsg resp;

    fd = open(req->pathname, O_RDONLY);
    // open failed, send error response
    if (fd == -1) {
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        exit(EXIT_FAILURE); // and terminate
    }

    // transmit file contents in messages with type RESP_MT_DATA.
    // we dont diagnose read() and msgsnd() errors since we cant notify the client
    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        if (msgsnd(req->clientId, &resp, numRead, 0) == -1) {
            break;
        }
    }

    // send a message of type RESP_MT_END
    resp.mtype = RESP_MT_END;
    msgsnd(req->clientId, &resp, 0, 0); // zero length mtext
}


int main(int argc, char *argv[])
{
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    int serverId;
    struct sigaction sa;

    // create server message queue
    serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverId == -1) {
        errExit("msgget");
    }

    // establish SIGCHLD handler to reap terminated children
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    // read requests, handle each in separate child process
    for (;;) {
        // get a request
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR) { // interrupted by SIGCHLD?
                continue;
            }

            errMsg("msgrcv"); // some other error
            break; // so terminate loop
        }

        pid = fork(); // create child process
        if (pid == -1) {
            errExit("fork");
            break;
        }

        if (pid == 0) {
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        // parent loops to receive next request
    }

    // if msgrcv() or fork() fails, remove server MQ and exit
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }

    exit(EXIT_SUCCESS);
}
