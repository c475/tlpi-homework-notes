#ifndef SYSV_SEQNUM_H

#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define SERVER_KEY 0xABCDEF // well known server MQ key

struct requestData {
    int clientId;
    unsigned int seqNum;
};

struct request {
    long mtype; // unused
    struct requestData data; // client message queue identifier + length of desired sequence
};

struct response {
    long mtype; // unused
    unsigned int seqNum; // start of sequence
};

#define REQ_MSG_SIZE sizeof(struct requestData)
#define RESP_MSG_SIZE sizeof(unsigned int)

#endif
