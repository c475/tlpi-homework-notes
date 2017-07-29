#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h> // offsetof
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <syslog.h>
#include "../../lib/tlpi_hdr.h"

#define DEFAULT_TIMEOUT 3 // default 3 second timeout on msgsnd() and msgrcv()

#define SERVER_KEY_FILE "/tmp/well_known_server_file" // "well-known" key for server message queue

struct requestMsg {             // request client->server
    long mtype;                 // unused
    int clientId;               // ID of the client's message queue
    char pathname[PATH_MAX];    // file to be returned
};

// need to use offsetof() to calculate the size of "mtext" because 
// of potential padding being added between clientID and pathname.
// why not use a substructure instead so you dont have to worry about the padding between them?
#define REQ_MSG_SIZE (offsetof(struct requestMsg, pathname) - offsetof(struct requestMsg, clientId) + PATH_MAX)

#define RESP_MSG_SIZE 8192

struct responseMsg {            // response server->client
    long mtype;                 // one of RESP_MT_* values below
    char data[RESP_MSG_SIZE];   // file content / response message
};

// types of responses that the server can send to the client
#define RESP_MT_FAILURE 1   // file couldn't be opened
#define RESP_MT_DATA    2   // message contains file data
#define RESP_MT_END     3   // File data complete
