#include <mqueue.h>
#include <fcntl.h>
#include <assert.h>
#include "../../lib/tlpi_hdr.h"

#define MESSAGE_MAX 4096
#define SERVER_MQ "/chat"
#define CLIENT_MQ_TEMPLATE "/%ld"
#define JOIN_TEMPLATE "%s has joined the chat\n"
#define CHAT_TEMPLATE "%s: %s\n"
#define CLIENT_MQ_NAME_MAX 32
#define NAME_MAX 32

#define JOIN 0
#define CHAT 1
#define LEAVE 2


struct request {
    int type;
    pid_t pid;
    char content[MESSAGE_MAX];
};


struct response {
    char content[MESSAGE_MAX];
};


struct client {
    mqd_t mq;
    pid_t pid;
    char name[NAME_MAX];
};


struct ChatPacket {
    struct client *client;
    struct response *response;
};


void logError(const char *msg, struct request *req)
{
    fprintf(stderr, "[%ld] %s\n", (long)req->pid, msg);
}


int findClient(void *arg, void *pid)
{
    assert(arg);
    assert(pid);

    struct client *c;
    pid_t *p;

    c = (struct client *)arg;
    p = (pid_t *)pid;

    return c->pid == *p;
}


int sendJoined(void *arg, void *resp)
{
    struct client *c;

    c = (struct client *)arg;

    mq_send(c->mq, (char *)resp, sizeof(struct response), 0);

    return 0;
}


int sendChat(void *arg, void *pack)
{
    struct ChatPacket *packet;
    struct client *c;

    packet = (struct ChatPacket *)pack;
    c = (struct client *)arg;

    if (c->pid != packet->client->pid) {
        mq_send(c->mq, (char *)packet->response, sizeof(struct response), 0);
    }

    return 0;
}


int clientJoin(struct List *list, struct request *req)
{
    struct client cl;
    struct response resp;
    char client_mq[CLIENT_MQ_NAME_MAX];

    assert(list);
    assert(req);

    cl.pid = req->pid;
    snprintf(client_mq, CLIENT_MQ_NAME_MAX, CLIENT_MQ_TEMPLATE, (long)req->pid);
    cl.mq = mq_open(client_mq, O_WRONLY | O_NONBLOCK);
    snprintf(cl.name, NAME_MAX, "%s", req->content);

    snprintf(resp.content, MESSAGE_MAX, JOIN_TEMPLATE, cl.name);

    listForeach(list, sendJoined, (void *)&resp);
    listAppend(list, &cl);

    return 0;
}


int clientChat(struct List *list, struct request *req, struct client *client)
{
    struct response resp;
    struct ChatPacket packet;

    assert(list);
    assert(req);
    assert(client);

    memset(resp.content, 0, MESSAGE_MAX);
    snprintf(resp.content, MESSAGE_MAX, CHAT_TEMPLATE, client->name, req->content);

    packet.client = client;
    packet.response = &resp;

    listForeach(list, sendChat, (void *)&packet);

    return 0;
}

// man that's enough
int main(int argc, char *argv[])
{
    mqd_t servermq;
    struct mq_attr attr;
    struct request req;
    struct List *clientList;
    struct client *client;

    memset(&attr, 0, sizeof(struct mq_attr));

    attr.mq_msgsize = sizeof(struct request);
    attr.mq_maxmsg = 10;

    servermq = mq_open(SERVER_MQ, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if (servermq == (mqd_t)-1) {
        errExit("mq_open");
    }

    clientList = listCreate(sizeof(struct client));
    if (clientList == NULL) {
        errExit("listCreate");
    }

    for (;;) {
        if (mq_receive(servermq, (char *)&req, sizeof(struct request), 0) == -1) {
            errExit("mq_receive");
        }

        printf("GOT REQ\n");

        switch (req.type) {
            // Add client to the list and round-robin "User joined" to all connected clients
            case JOIN:
                if (clientJoin(clientList, &req) == -1) {
                    logError("Client join failed", &req);
                }
                break;

            // Round-robin message to all connected clients
            case CHAT:
                client = listFindFirst(clientList, findClient, (void *)&req.pid);
                if (!client) {
                    logError("Couldnt find client", &req);
                } else if (clientChat(clientList, &req, client) == -1) {
                    logError("Client chat failed", &req);
                }
                break;

            // Client sent malformed request
            default:
                logError("Bad request", &req);
                break;
        }
    }

    if (mq_close(servermq) == -1) {
        errExit("mq_close");
    }

    if (mq_unlink(SERVER_MQ) == -1) {
        errExit("mq_unlink");
    }

    listDestroy(clientList);

    exit(EXIT_SUCCESS);
}
