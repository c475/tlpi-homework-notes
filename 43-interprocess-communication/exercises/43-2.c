#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
// system v headers
#include <sys/msg.h>

#include "../../lib/tlpi_hdr.h"


struct SVMsg {
    long mtype;
    char *data;
};


void test_systemv_mq(int blockCount, int blockSize)
{
    int i;
    int mqid;
    struct SVMsg msg;
    char *block;
    pid_t childPid;
    key_t mqkey;

    // just hardcode the key, no system v message queues are on my system
    mqkey = 400;
    mqid = msgget(mqkey, S_IRUSR | S_IWUSR | IPC_CREAT);
    if (mqid == -1) {
        errExit("msgget");
    }

    block = calloc(blockCount * blockSize, 1);

    msg.mtype = 1;
    msg.data = block;

    switch (childPid = fork()) {

        case -1:
            errExit("fork");

        // child sends messages
        case 0:

            for (i = 0; i < blockCount; i++) {
                if (msgsnd(mqid, &msg, blockSize, 0) == -1) {
                    errExit("msgsnd");
                }
            }

            _exit(EXIT_SUCCESS);

        // parent consumes messages
        default:

            for (i = 0; i < blockCount; i++) {
                if (msgrcv(mqid, &msg, blockSize, 0, 0) != blockSize) {
                    errExit("msgrcv");
                }
            }

            wait(NULL);
            break;
    }

    if (msgctl(mqid, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}


void test_posix_mq(int blockCont, int blockSize)
{

}


void test_unix_stream(int blockCont, int blockSize)
{

}


void test_unix_datagram(int blockCont, int blockSize)
{

}


// test the bandwidth of pipe()s
int main(int argc, char *argv[])
{
    int blockCount;
    int blockSize;

    if (argc != 3) {
        usageErr("%s num-blocks block-size\n", argv[0]);
    }

    blockCount = getInt(argv[1], GN_NONNEG, 0);
    blockSize = getInt(argv[2], GN_NONNEG, 0);

    test_systemv_mq(blockCount, blockSize);

    printf("Success somehow\n");

    exit(EXIT_SUCCESS);
}
