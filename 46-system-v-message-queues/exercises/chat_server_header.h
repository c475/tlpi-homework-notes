#ifndef CHAT_SERVER_HEADER
#define CHAT_SERVER_HEADER

#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


#define SERVER_KEY 0xABCDEF

// 2 second timeout on message queues
#define DEFAULT_TIMEOUT 2

#define MAX_NAME 17
#define TEXT_SIZE 1024

#define CLIENT_JOIN_OR_LEAVE 1
#define CLIENT_CHAT 2

#define SERVER_JOIN_OR_LEAVE 3
#define SERVER_CHAT 4

#define META_JOIN 5
#define META_LEAVE 6
#define META_OK 7
#define META_ERR 8


struct Packet {
    int meta; // META_JOIN | META_LEAVE
    int mqid; // a message queue id, not always used
    char text[TEXT_SIZE];
};

struct Message {
    long mtype;
    struct Packet data;
};


#define MESSAGE_SIZE sizeof(struct Packet)

#endif
