#include <mqueue.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define MESSAGE_MAX 4096
#define SERVER_MQ "/chat"
#define CLIENT_MQ_TEMPLATE "/%ld"
#define JOIN_MESSAGE_TEMPLATE "%s has joined the chat\n"
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
    mqd_t *mq;
    pid_t pid;
    char name[NAME_MAX];
};


void logError(const char *msg, struct request *req)
{
    fprintf(stderr, "[%ld] %s\n", (long)req->pid, msg);
}


void sendJoinMessage(void *client, void *resp)
{
    struct client *cl;

    cl = (struct client *)client;

    mq_send(cl->mq, (char *)resp, sizeof(struct response), 0);
}


void sendChatMessage(void *client, void *resp)
{
    
}


int clientJoin(struct request *req, struct List *list)
{
    mqd_t mq;
    struct client cl;
    char client_mq[CLIENT_MQ_NAME_MAX];
    struct response resp;

    cl.pid = req->pid;
    memset(cl.name, 0, NAME_MAX);
    memset(resp.content, 0, MESSAGE_MAX);
    snprintf(cl.name, NAME_MAX, "%s", req->content);
    snprintf(client_mq, CLIENT_MQ_NAME_MAX, CLIENT_MQ_TEMPLATE, (long)cl.pid);
    snprintf(resp.content, MESSAGE_MAX, JOIN_MESSAGE_TEMPLATE, cl.name);

    mq = mq_open(client_mq, O_WRONLY | O_NONBLOCK);
    if (mq == (mqd_t)-1) {
        return -1;
    }

    cl.mq = mq;

    listForeach(list, sendJoinMessage, &resp);
    listAdd(list, &cl);

    return 0;
}


int clientChat(struct request *req, struct List *list)
{
    listForeach()
}



int main(int argc, char *argv[])
{
    mqd_t servermq;
    struct mq_attr attr;
    struct request req;
    struct response resp;
    struct List *clients;

    memset(&attr, 0, sizeof(struct mq_attr));

    attr.mq_msgsize = sizeof(struct request);
    attr.mq_maxmsg = 10;

    servermq = mq_open(SERVER_MQ, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if (servermq == (mqd_t)-1) {
        errExit("mq_open");
    }

    clients = listCreate(sizeof(struct client));
    if (clients == NULL) {
        errExit("listCreate");
    }

    for (;;) {
        if (mq_receive(servermq, (char *)&req, sizeof(struct request), 0) == -1) {
            errExit("mq_receive");
        }

        switch (req.type) {
            // Add client to the list and round-robin "User joined" to all connected clients
            case JOIN:
                if (clientJoin(&req, clients) == -1) {
                    logError("Client join failed", &req);
                }
                break;

            // Round-robin message to all connected clients
            case CHAT:
                if (clientChat(&req, clients) == -1) {
                    logError("Client chat failed", &req);
                }
                break;

            // Round-robin "User left" to all connected clients
            case LEAVE:
                if (clientLeave(&req, clients) == -1) {
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

    listDestroy(list);

    exit(EXIT_SUCCESS);
}