#include "chat_server_header.h"


static int serverId;
static int clientId;


void cleanup(void)
{
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}
