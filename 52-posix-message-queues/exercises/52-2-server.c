#include <mqueue.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define SERVER_MQ_NAME "/seqnum"
#define MAX_NAME 32
#define CLIENT_MQ_TEMPLATE "/%ld"


struct request {
    pid_t pid;
    unsigned int num; // length of desired sequence
};

struct response {
    unsigned int num; // start of sequence
};


int main(int argc, char *argv[])
{
    int num;
    mqd_t server_mq;
    mqd_t client_mq;
    char client_mq_name[MAX_NAME];
    struct mq_attr server_attr;
    struct response *resp;
    struct request req;

    // the "service"
    num = 0;

    // setup server attributes
    memset(&server_attr, 0, sizeof(struct mq_attr));
    server_attr.mq_msgsize = sizeof(struct request);
    server_attr.mq_maxmsg = 10; // can queue up 10 requests

    // REALLY WANT TO REMEMBER THIS: YOU CAN ONLY CHANGE THE mq_flags FIELD WITH mq_setattr()!!!!
    server_mq = mq_open(SERVER_MQ_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &server_attr);
    if (server_mq == (mqd_t)-1) {
        errExit("mq_open");
    }

    resp = malloc(sizeof(struct response));
    if (resp == NULL) {
        errExit("malloc");
    }

    for (;;) {
        // get request
        if (mq_receive(server_mq, (char *)&req, sizeof(struct request), 0) == -1) {
            errExit("mq_receive");
        }

        memset(client_mq_name, 0, MAX_NAME);
        snprintf(client_mq_name, MAX_NAME, CLIENT_MQ_TEMPLATE, (long)req.pid);

        // try to open the client mq
        // I think it's smart to open nonblock here? Can prevent DOS.
        client_mq = mq_open(client_mq_name, O_WRONLY | O_NONBLOCK);
        if (client_mq == (mqd_t)-1) {
            fprintf(stderr, "Failed to open client mq %s\n", client_mq_name);
            continue;
        }

        resp->num = num;

        // send response
        if (mq_send(client_mq, (char *)resp, sizeof(struct response), 0) == -1) {
            fprintf(stderr, "Failed to write to client mq %s\n", client_mq_name);
            continue;
        }

        // close client mq
        if (mq_close(client_mq) == -1) {
            fprintf(stderr, "Couldn't close client mq %s\n", client_mq_name);
            continue;
        }

        num += req.num;
    }

    // shouldn't happen but whatever

    if (mq_close(server_mq) == -1) {
        errExit("mq_close");
    }

    free(resp);

    exit(EXIT_SUCCESS);
}
