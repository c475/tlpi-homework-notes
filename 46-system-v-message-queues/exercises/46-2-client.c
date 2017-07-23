#include "seqnum_header.h"


static int clientId;


static void removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}


int main(int argc, char *argv[])
{
    int serverId;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [seq-len...]\n", argv[0]);
    }

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
        errExit("msgget - server");
    }

    // create the client message queue
    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1) {
        errExit("msgget - client");
    }

    // clean up the client's message queue
    if (atexit(removeQueue) != 0) {
        errExit("atexit");
    }

    req.mtype = 1;
    req.data.clientId = clientId;

    if (argc == 1) {
        req.data.seqNum = 1;
    } else {
        req.data.seqNum = getInt(argv[1], GN_NONNEG, "req.data.seqNum");
    }

    // send request
    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) {
        errExit("msgsnd");
    }

    // wait for response
    if (msgrcv(clientId, &resp, RESP_MSG_SIZE, 1, 0) == -1) {
        errExit("msgrcv");
    }

    printf("%d\n", resp.seqNum);

    exit(EXIT_SUCCESS);
}
