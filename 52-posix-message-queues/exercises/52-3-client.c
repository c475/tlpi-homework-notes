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
    mqd_t clientmq;
    mqd_t servermq;
    struct request req;
    struct response resp;
    char client_mq_name[NAME_SIZE];
    struct mq_attr attr;

    if (argc != 2) {
        usageErr("%s pathname\n", argv[0]);
    }

    memset(client_mq_name, 0, NAME_SIZE);
    memset(&attr, 0, sizeof(struct mq_attr));
    memset(&req, 0, sizeof(struct request));

    snprintf(client_mq_name, NAME_SIZE, CLIENT_MQ_TEMPLATE, (long)getpid());

    attr.mq_msgsize = sizeof(struct response);
    attr.mq_maxmsg = 10;

    clientmq = mq_open(client_mq_name, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, &attr);
    if (clientmq == (mqd_t)-1) {
        errExit("mq_open (client)");
    }

    req.pid = getpid();
    strncpy(req.pathname, argv[1], PATH_MAX);

    servermq = mq_open(SERVER_MQ, O_WRONLY);
    if (servermq == (mqd_t)-1) {
        errExit("mq_open (server)");
    }

    // send request for a file to the server
    if (mq_send(servermq, (char *)&req, sizeof(struct request), 0) == -1) {
        errExit("mq_send");
    }

    // read all chunks of response until the server says it's done
    for (;;) {
        if (mq_receive(clientmq, (char *)&resp, sizeof(struct response), 0) == -1) {
            errExit("mq_receive");
        }

        // server sends a chunk starting with a null byte when it's done
        if (resp.contents[0] == '\0') {
            break;
        }

        printf("%s", resp.contents);
    }

    if (mq_close(servermq) == -1) {
        errExit("mq_close (server)");
    }

    if (mq_close(clientmq) == -1) {
        errExit("mq_close (client)");
    }

    if (mq_unlink(client_mq_name) == -1) {
        errExit("mq_unlink");
    }

    exit(EXIT_SUCCESS);
}