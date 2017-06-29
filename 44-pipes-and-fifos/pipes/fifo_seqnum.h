#ifndef FIFO_SEQNUM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define SERVER_FIFO "/tmp/seqnum_sv" // well known name for seqnum server

#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld" // template for building client FIFO 

#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20) // size of template + 20 for PID (generous)

struct request {
    pid_t pid; // pid of client
    unsigned int seqLen; // length of desired sequence
};

struct response {
    unsigned int seqNum; // start of sequence
};

#endif
