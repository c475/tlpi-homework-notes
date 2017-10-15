#include "chat_server_header.h"


static int clientId = -1;
static int serverId = -1;


void timeoutHandler(int sig)
{
    // do nothing
}


void startTimeout(void)
{

}


void endTimeout(void)
{

}


void cleanup(void)
{
    struct Message message;
    message.mtype = CLIENT_JOIN_OR_LEAVE;
    message.data.meta = META_LEAVE;
    memset(message.data.text, 0, TEXT_SIZE);

    if (serverId != -1 && msgsnd(serverId, &message, MESSAGE_SIZE, 0) == -1) {
        errExit("msgsnd");
    }

    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}


// child process that displays when users join or leave
void handleRoomStatus(void)
{
    struct Message message;

    for (;;) {
        if (msgrcv(clientId, &message, MESSAGE_SIZE, SERVER_JOIN_OR_LEAVE, 0) == -1) {
            fprintf(stderr, "handleRoomStatus fucked up\n");
            break;
        }

        printf("%s\n", message.data.text);
    }

    exit(EXIT_FAILURE);
}


// child process that displays chat messages from users
void handleChats(void)
{
    struct Message message;

    for (;;) {
        if (msgrcv(clientId, &message, MESSAGE_SIZE, SERVER_CHAT, 0) == -1) {
            fprintf(stderr, "handleChats fucked up\n");
            break;
        }

        printf("%s\n", message.data.text);
    }

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    size_t len;
    char name[MAX_NAME];
    pid_t childPid;
    struct Message message;

    memset(name, 0, MAX_NAME);

    if (atexit(cleanup) != 0) {
        errExit("atexit");
    }

    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
    if (clientId == -1) {
        errExit("client msgget");
    }

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
        errExit("server msgget");
    }

    if (fgets(name, MAX_NAME, stdin) == NULL) {
        errExit("fgets");
    }

    len = strlen(name);
    name[len - 1] = '\0';

    message.mtype = CLIENT_JOIN_OR_LEAVE;
    message.data.meta = META_JOIN;
    message.data.mqid = clientId;
    memset(message.data.text, 0, TEXT_SIZE);
    strncpy(message.data.text, name, len);

    // request to join the chat server
    if (msgsnd(serverId, &message, MESSAGE_SIZE, 0) == -1) {
        errExit("msgsnd");
    }

    // see if the server agrees
    if (msgrcv(clientId, &message, MESSAGE_SIZE, SERVER_JOIN_OR_LEAVE, 0) == -1) {
        errExit("msgsnd");
    }

    if (message.data.meta == META_ERR) {
        fprintf(stderr, "Failed to join the server\n");
        exit(EXIT_FAILURE);
    } 

    // joining went ok, start up client handlers and enter message loop

    childPid = fork();
    if (childPid == 0) {
        handleChats();
    }

    childPid = fork();
    if (childPid == 0) {
        handleRoomStatus();
    }

    message.mtype = CLIENT_CHAT;
    message.data.meta = 0;
    message.data.mqid = clientId;
    while ((fgets(message.data.text, TEXT_SIZE, stdin)) != NULL) {
        len = strlen(message.data.text);
        message.data.text[len - 1] = '\0';

        if (msgsnd(serverId, &message, MESSAGE_SIZE, 0) == -1) {
            errExit("msgsnd");
        }
    }

    exit(EXIT_SUCCESS);
}
