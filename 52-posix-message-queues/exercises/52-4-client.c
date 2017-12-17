#include <mqueue.h>
#include <fcntl.h>
#include <pthread.h>
#include "../../lib/tlpi_hdr.h"

#define MESSAGE_MAX 4096
#define SERVER_MQ "/chat"
#define CLIENT_MQ_TEMPLATE "/%ld"
#define NAME_MAX 32
#define CLIENT_MQ_NAME_MAX 32

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


void *handleMessages(void *c)
{
    struct client *client;
    struct response resp;

    client = (struct client *)c;

    for (;;) {
        if (mq_receive(client->mq, (char *)&resp, sizeof(struct response), 0) == -1) {
            errExit("mq_receive");
        }

        printf("%s", resp.content);
    }

    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_t messageThread;
    mqd_t servermq;
    struct client cl;
    char client_mq[CLIENT_MQ_NAME_MAX];
    struct mq_attr attr;
    struct request req;
    struct response resp;

    memset(client_mq, 0, CLIENT_MQ_NAME_MAX);
    memset(&attr, 0, sizeof(struct mq_attr));
    memset(&req, 0, sizeof(struct request));
    memset(&resp, 0, sizeof(struct response));
    snprintf(client_mq, CLIENT_MQ_NAME_MAX, CLIENT_MQ_TEMPLATE, (long)getpid());
    snprintf(req.content, MESSAGE_MAX, "%s", "woo");

    req.pid = getpid();
    req.type = JOIN;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct response);

    cl.mq = mq_open(client_mq, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, &attr);
    if (cl.mq == (mqd_t)-1) {
        errExit("mq_open");
    }

    snprintf(cl.name, NAME_MAX, "%s", "woo");
    cl.pid = getpid();

    servermq = mq_open(SERVER_MQ, O_WRONLY);
    if (servermq == (mqd_t)-1) {
        errExit("mq_open");
    }

    if (mq_send(servermq, (char *)&req, sizeof(struct request), 0) == -1) {
        errExit("mq_send");
    }

    if (pthread_create(&messageThread, NULL, handleMessages, &cl) != 0) {
        errExit("pthread_create");
    }

    req.type = CHAT;

    for (;;) {
        memset(req.content, 0, MESSAGE_MAX);
        read(STDIN_FILENO, req.content, MESSAGE_MAX - 1);

        if (mq_send(servermq, (char *)&req, sizeof(struct request), 0) == -1) {
            errExit("mq_send");
        }
    }

    if (pthread_join(messageThread, NULL) != 0) {
        errExit("pthread_join");
    }

    printf("%s", resp.content);

    mq_close(servermq);
    mq_close(cl.mq);
    mq_unlink(client_mq);

    exit(EXIT_SUCCESS);
}
