#include <mqueue.h>
#include <fcntl.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"

#define CLIENT_MQ_TEMPLATE "/%ld"
#define NAME_SIZE 32
#define MESSAGE_SIZE 8192
#define SERVER_MQ "/fileserver"


struct request {
    pid_t pid;
    char pathname[PATH_MAX];
};

struct response {
    char contents[MESSAGE_SIZE];
};


int main(int argc, char *argv[])
{
    int fd;
    int readReturn;
    mqd_t servermq;
    mqd_t clientmq;
    struct mq_attr attr;
    struct request req;
    struct response resp;
    char client_mq_name[NAME_SIZE];

    memset(&attr, 0, sizeof(struct mq_attr));

    attr.mq_msgsize = sizeof(struct request);
    attr.mq_maxmsg = 10;

    servermq = mq_open(SERVER_MQ, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr);
    if (servermq == (mqd_t)-1) {
        errExit("mq_open");
    }

    for (;;) {
        // read a request
        if (mq_receive(servermq, (char *)&req, sizeof(struct request), 0) == -1) {
            errExit("mq_receive");
        }

        snprintf(client_mq_name, NAME_SIZE, CLIENT_MQ_TEMPLATE, (long)req.pid);

        // open the client message queue
        // turns out O_NONBLOCK doesn't work so well when you're dealing with multiple chunks of data
        // better to thread it and use mq_timedsend and mq_timedreceive at that point...
        clientmq = mq_open(client_mq_name, O_WRONLY);
        if (clientmq == (mqd_t)-1) {
            fprintf(stderr, "Could Not open client mq %s\n", client_mq_name);
            continue;
        }

        // open the requested file
        fd = open(req.pathname, O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "Could not open file %s\n", req.pathname);
            mq_close(clientmq);
            continue;
        }

        memset(resp.contents, 0, MESSAGE_SIZE);

        // read chunks from the file and send them to the client until it's all gone
        while ((readReturn = read(fd, resp.contents, MESSAGE_SIZE - 1)) > 0) {
            if (mq_send(clientmq, (char *)&resp, sizeof(struct response), 0) == -1) {
                fprintf(stderr, "Could not send response to %s\n", client_mq_name);
                break;
            }
        }

        // signifies the server is done with the client
        resp.contents[0] = '\0';

        if (mq_send(clientmq, (char *)&resp, sizeof(struct response), 0) == -1) {
            fprintf(stderr, "Could not terminate client session %s\n", client_mq_name);
        }

        if (mq_close(clientmq) == -1) {
            fprintf(stderr, "Error closing client mq %s\n", client_mq_name);
        }

        close(fd);
    }

    if (mq_unlink(SERVER_MQ) == -1) {
        errExit("mq_unlink");
    }

    if (mq_close(servermq) == -1) {
        errExit("mq_close");
    }

    exit(EXIT_SUCCESS);
}
