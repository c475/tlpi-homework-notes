#ifndef CHAT_SERVER_HEADER
#define CHAT_SERVER_HEADER

#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"

// 2 second timeout on message queues
#define DEFAULT_TIMEOUT 2

#define MAX_MESSAGE 1024
#define MAX_NAME 17

// various "mtype" values
#define RESP_OK 1
#define RESP_ERR 2
#define RESP_CHAT 3 // chat message
#define RESP_STATUS 4 // notifications for users leaving and joining the server
#define REQ_NAME 5 // sent to server when registering a name
#define REQ_CHAT 6 // sent to server when the client sends a message

// "well-known" server key
#define SERVER_KEY 0xABCDEF

#define MESSAGE_SIZE (1 << 20) / 2 // 0.5 MB

// it's easier to just msgrcv() on this then cast to the appropriate type based on the "mtype" field...
struct message {
    long mtype;
    char MESSAGE_PADDING[MESSAGE_SIZE];
};

struct chatRequestData {
    int clientId;
    char text[MAX_MESSAGE];
};

struct chatRequest {
    long mtype;
    struct chatRequestData data;
};

struct chatResponseData {
    char name[MAX_NAME];
    char text[MAX_MESSAGE];
};

struct chatResponse {
    long mtype;
    struct chatResponseData data;
};

struct nameRequestData {
    int clientId;
    char name[MAX_NAME];
};

struct nameRequest {
    long mtype;
    struct nameRequestData data;
};


#define USER_LEFT 0
#define USER_JOINED 1
struct serverStatusData {
    char name[MAX_NAME];
    int type; // USER_LEFT || USER_JOINED || RESP_OK || RESP_ERR
};

struct serverStatus {
    long mtype;
    struct serverStatusData data;
};


#endif
