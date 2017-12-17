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
    mqd_t server_mq;
    mqd_t client_mq;
    struct request req;
    struct response resp;
    struct mq_attr client_attr;
    char client_mq_name[MAX_NAME];

    if (argc != 2) {
        usageErr("%s num\n", argv[0]);
    }

    req.pid = getpid();
    req.num = getLong(argv[1], GN_NONNEG, "num");

    memset(client_mq_name, 0, MAX_NAME);
    snprintf(client_mq_name, MAX_NAME, CLIENT_MQ_TEMPLATE, (long)req.pid);

    // setup client message queue with correct attributes
    memset(&client_attr, 0, sizeof(struct mq_attr));
    client_attr.mq_msgsize = sizeof(struct response);
    client_attr.mq_maxmsg = 1; // can only have a single message queued (the server's response)

    client_mq = mq_open(client_mq_name, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, &client_attr);
    if (client_mq == (mqd_t) -1) {
        errExit("mq_open (client)");
    }

    server_mq = mq_open(SERVER_MQ_NAME, O_WRONLY);
    if (server_mq == (mqd_t)-1) {
        errExit("mq_open (server)");
    }

    // send request
    if (mq_send(server_mq, (char *)&req, sizeof(struct request), 0) == -1) {
        errExit("mq_send");
    }

    // wait for resposne
    if (mq_receive(client_mq, (char *)&resp, sizeof(struct response), 0) == -1) {
        errExit("mq_receive");
    }

    if (mq_close(server_mq) == -1) {
        errExit("mq_close (server)");
    }

    if (mq_close(client_mq) == -1) {
        errExit("mq_close (client)");
    }

    // make sure to get rid of the client mq so it doesnt stay on the filesystem
    if (mq_unlink(client_mq_name) == -1) {
        errExit("mq_unlink");
    }

    printf("[%ld] num: %d\n", (long)getpid(), resp.num);

    exit(EXIT_SUCCESS);
}
