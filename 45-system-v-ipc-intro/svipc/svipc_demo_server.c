#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "../../lib/tlpi_hdr.h"

#define KEY_FILE "/some-path/some-file" // should be an existing file, or one the server creates


int main(int argc, char *argv[])
{
    int msqid;
    key_t key;
    const int MQ_PERMS = S_IRUSR | S_IWUSR | S_IWGRP; // rw--w----

    // optional code here to check if another server process is already running

    // generate the key file for the message queue
    key = ftok(KEY_FILE, 1);
    if (key == -1) {
        errExit("ftok");
    }

    // while msgget() fails, try recreating the message queue exclusively
    while ((msqid = msgget(key, IPC_CREAT | IPC_EXC | MQ_PERMS)) == -1) {
        // message queue with the same key already exists, remove it and try again
        if (errno == EEXIST) {
            msqid = msgget(key, 0);
            if (msqid == -1) {
                errExit("msgget() failed to retrieve old queue ID");
            }

            if (msgctl(msqid, IPC_RMID, NULL) == -1) {
                errExit("msgget() failed to delete old queue");
            }

            printf("Removed old message queue (id=%d)\n", msqid);

        // some other error, give up
        } else {
            errExit("msgget() failed");
        }
    }

    // upon loop exit, we've successfully created the message queue and can carry on to do other work

    exit(EXIT_SUCCESS);
}
