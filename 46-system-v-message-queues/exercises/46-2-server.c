#include "seqnum_header.h"


// surprisingly cleaner than the FIFO server
// the dummy file descriptor method is pretty messy
int main(int argc, char *argv[])
{
    int seqNum;
    int serverId;
    struct request req;
    struct response resp;

    seqNum = 0;

    // create the server message queue
    serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverId == -1) {
        errExit("msgget - server");
    }

    resp.mtype = 1;

    // iterative, no need to fork or thread to increment an int
    for (;;) {
        if (msgrcv(serverId, &req, REQ_MSG_SIZE, 1, 0) == -1) {
            errMsg("Problem reading request, bailing");
            break;
        }

        resp.seqNum = seqNum;

        if (msgsnd(req.data.clientId, &resp, RESP_MSG_SIZE, 0) == -1) {
            errMsg("Problem sending response, bailing");
            break;
        }

        seqNum += req.data.seqNum;
    }

    // delete the server message queue
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }

    exit(EXIT_SUCCESS);
}
