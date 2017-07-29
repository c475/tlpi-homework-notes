#include "file_server_header.h"


static int clientId;


// SIGALRM
static void timeoutHandler(int sig)
{
    // do nothing
}


static void removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}

// wrapper for msgsnd() with a timeout feature in case of server error
// returns 0 on success, -1 on error
int sendMessage(int mqid, const struct requestMsg *req, size_t size, int timeout)
{
    int ret;
    int savedErrno;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = timeoutHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        return -1;
    }

    if (timeout > 0) {
        alarm(timeout);
    }

    ret = msgsnd(mqid, req, size, 0);

    if (timeout > 0) {
        savedErrno = errno;
        alarm(0);
        errno = savedErrno;
    }

    signal(SIGALRM, SIG_DFL);

    return ret;
}

// wrapper for mscrcv() with a timeout feature in case of server error
// also places the response in the "resp" argument
// returns number of bytes read on success, -1 on error
int receiveMessage(int mqid, struct responseMsg *resp, size_t size, int timeout)
{
    int ret;
    int savedErrno;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = timeoutHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        return -1;
    }

    if (timeout > 0) {
        alarm(timeout);
    }

    ret = msgrcv(mqid, resp, size, 0, 0);

    if (timeout > 0) {
        savedErrno = errno;
        alarm(0);
        errno = savedErrno;
    }

    signal(SIGALRM, SIG_DFL);

    return ret;
}


int main(int argc, char *argv[])
{
    int fd;
    struct requestMsg req;
    struct responseMsg resp;
    int serverId;
    int numMsgs;
    ssize_t msgLen;
    ssize_t totBytes;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s pathname\n", argv[0]);
    }

    if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
        cmdLineErr("pathname too long (max: %ld bytes)\n", (long)sizeof(req.pathname) - 1);
    }

    // get server's queue identifier; create queue for response
    fd = open(SERVER_KEY_FILE, O_RDONLY);
    if (fd == -1) {
        errExit("open - server key file");
    }

    if (read(fd, &serverId, sizeof(serverId)) != sizeof(serverId)) {
        errExit("read - server key file");
    }

    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1) {
        errExit("msgget - client message queue");
    }

    if (atexit(removeQueue) != 0) {
        errExit("atexit");
    }

    // send request asking for file named in argv[1]
    req.mtype = 1; // any type will do
    req.clientId = clientId;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0'; // ensure string is null terminated

    // send initial request
    if (sendMessage(serverId, &req, REQ_MSG_SIZE, DEFAULT_TIMEOUT) == -1) {
        if (errno == EINTR) {
            fprintf(stderr, "Server timed out. Exiting.\n");
        }
        errExit("sendMessage");
    }

    // get first response, which may be failure notification
    msgLen = receiveMessage(clientId, &resp, RESP_MSG_SIZE, DEFAULT_TIMEOUT);
    if (msgLen == -1) {
        if (errno == EINTR) {
            fprintf(stderr, "Server timed out. Exiting.\n");
        }
        errExit("receiveMessage");
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data); // display msg from server
        exit(EXIT_FAILURE);
    }

    // file opened successfully from server
    // process messages (including the one already received) containing file data

    totBytes = msgLen; // count first message
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = receiveMessage(clientId, &resp, RESP_MSG_SIZE, DEFAULT_TIMEOUT);
        if (msgLen == -1) {
            if (errno == EINTR) {
                fprintf(stderr, "Server timed out. Exiting.\n");
            }
            errExit("receiveMessage");
        }

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%d messages)\n", (long)totBytes, numMsgs);

    exit(EXIT_SUCCESS);
}
