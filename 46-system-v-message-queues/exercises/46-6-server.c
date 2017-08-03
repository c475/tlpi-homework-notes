#include "chat_server_header.h"


static int serverId;


struct client {
    int clientId;
    char name[MAX_NAME];
    struct client *prev;
    struct client *next;
};

struct ClientList {
    unsigned int len;
    struct client *base;
};


static struct ClientList *CLIENTS = NULL;


void cleanup(void)
{
    struct client *client_p;
    struct client *prev;

    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }

    client_p = CLIENTS->base;
    while (client_p != NULL) {
        prev = client_p;
        client_p = client_p->next;
        free(prev);
    }

    free(CLIENTS);
}


void addClient(int clientId, const char *name)
{
    struct client *c;
    struct client *end;

    c = calloc(1, sizeof(struct client));
    if (c == NULL) {
        errExit("calloc");
    }

    c->clientId = clientId;
    strncpy(c->name, name, MAX_NAME);
    c->name[MAX_NAME - 1] = '\0';

    end = CLIENTS->base;
    while (end->next != NULL) {
        end = end->next;
    }

    end->next = c;
    c->prev = end;
    c->next = NULL;
}


void removeClient(int clientId)
{
    struct client *c;

    // 0 length list, no client to remove
    if (CLIENTS->base == NULL) {
        return;
    }

    // client is first in the list
    c = CLIENTS->base;
    if (c->clientId == clientId) {
        if (c->next != NULL) {
            c->next->prev = NULL;
            CLIENTS->base = c->next;
        } else {
            CLIENTS->base = NULL;
        }

        free(c);

        return;
    }

    // client is not first in the list
    c = c->next;
    while (c != NULL) {
        if (c->clientId == clientId) {
            if (c->next == NULL) {
                c->prev->next = NULL;
            } else {
                c->prev->next = c->next;
                c->next->prev = c->prev;
            }

            free(c);
        }
    }
}


void distributeMessage(struct chatRequest *request)
{
    int res;
    struct client *c;
    struct chatResponse response;

    response.mtype = RESP_CHAT;

    strncpy(response.data.name, request->data.name, MAX_NAME);
    response.data.name[MAX_NAME - 1] = '\0';
    strncpy(response.data.text, request->data.text, MAX_MESSAGE);
    response.data.text[MAX_MESSAGE - 1] = '\0';

    c = CLIENTS->base;
    while (c != NULL) {
        startTimeout();
        res = msgsnd(c->clientId, &response, MESSAGE_SIZE, 0);
        endTimeout();

        if (res == -1) {
            // remove the faulty client
            removeClient(c->clientId);
        }
    }
}


int handleJoin(struct message *request)
{
    struct nameRequest *nr;
    struct serverStatus status;
    struct client *c;

    nr = (struct nameRequest *)request;
    status = { 0 };

    // send an error back to the client letting them know their name selection is bad
    if (addClient(nr->clientId, nr->data->name) == -1) {
        status.mtype = RESP_ERR;
        msgsnd(nr->clientId, &status, MESSAGE_SIZE, 0);
        return -1;
    }

    status.mtype = RESP_STATUS;
    status.data.type = USER_JOINED;
    strncpy(status.data.name, nr->data->name, NAME_MAX);
    status.data.name[NAME_MAX = 1] = '\0';

    // broadcast that a user has joined the server
    c = CLIENTS.base;
    while (c != NULL) {
        if (c->clientId != nr->clientId) {
            startTimeout();
            msgsnd(c->clientId, &status, MESSAGE_SIZE, 0);
            endTimeout();
        }
    }

    return 0;
}


void handleLeave(struct message *request)
{
    struct nameRequest *nr;
    struct serverStatus status;
    struct client *c;

    nr = (struct nameRequest *)request;
    status = { 0 };

    // send an error back to the client letting them know their name selection is bad
    if (addClient(nr->clientId, nr->data->name) == -1) {
        status.mtype = RESP_ERR;
        msgsnd(nr->clientId, &status, MESSAGE_SIZE, 0);
        return -1;
    }

    status.mtype = RESP_STATUS;
    status.data.type = USER_JOINED;
    strncpy(status.data.name, nr->data->name, NAME_MAX);
    status.data.name[NAME_MAX = 1] = '\0';

    // broadcast that a user has joined the server
    c = CLIENTS.base;
    while (c != NULL) {
        if (c->clientId != nr->clientId) {
            startTimeout();
            msgsnd(c->clientId, &status, MESSAGE_SIZE, 0);
            endTimeout();
        }
    }

    return 0;
}


void handleChat(struct message *request)
{

}


// went the iterative route until I get to the server design chapter
// too many tricky things to worry about with a threaded server
int main(int argc, char *argv[])
{
    int res;
    pid_t childPid;
    struct message m;

    CLIENTS = calloc(1, sizeof(struct clientsList));
    if (CLIENTS == NULL) {
        errExit("calloc");
    }

    if (atexit(cleanup) != 0) {
        errExit("atexit");
    }

    // if (becomeDaemon(0) == -1) {
    //     errExit("becomeDaemon");
    // }

    // main server loop
    for (;;) {
        res = msgrcv(serverId, &m, MESSAGE_SIZE, 0);
        if (res == -1) {
            errExit("msgrcv");
        }

        childPid = fork();
        if (childPid == 0) {
            switch (res.mtype) {

                case REQ_JOIN:
                    handleJoin(&m);
                    break;

                case REQ_LEAVE:
                    handleLeave(&m);
                    break;

                case REQ_CHAT:
                    handleChat(&m);
                    break;

                // unknown request type, ignore
                default:
                    break;
            }
        }
    }

    exit(EXIT_SUCCESS);
}
