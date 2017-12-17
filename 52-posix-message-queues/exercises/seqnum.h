#ifndef _SEQNUM_HEADER_
#define _SEQNUM_HEADER_

#define MQ_NAME "/seqnum"

struct request {
    pid_t pid; // pid of client
    unsigned int num; // length of desired sequence
};

struct response {
    unsigned int num; // start of sequence
};

#endif
