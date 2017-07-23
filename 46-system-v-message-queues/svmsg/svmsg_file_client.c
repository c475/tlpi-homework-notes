#include "svmsg_file.h"


static int clientId;


static void removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}


int main(int argc, char *argv[])
{
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
    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
        errExit("msgget - server message queue");
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

    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) {
        errExit("msgsnd");
    }

    // get first response, which mat be failure notification
    msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
    if (msgLen == -1) {
        errExit("msgrcv");
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data); // display msg from server
        exit(EXIT_FAILURE);
    }

    // file opened successfully from server
    // process messages (including the one already received) containing file data

    totBytes = msgLen; // count first message
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            errExit("msgrcv");
        }

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%d messages)\n", (long)totBytes, numMsgs);

    exit(EXIT_SUCCESS);
}
