#include "chat_server_header.h"


static int clientId;
static int serverId;


void timeoutHandler(int sig)
{
    // do nothing
}


void cleanup(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }
}


void startTimeout(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = timeoutHandler;
    sigaction(SIGALRM, &sa, NULL);
    alarm(DEFAULT_TIMEOUT);
}


void endTimeout(void)
{
    alarm(0);
    signal(SIGALRM, SIG_DFL);
}


int receiveChat(struct chatResponse *response)
{
    int ret;

    startTimeout();

    ret = msgrcv(clientId, response, REQ_SIZE_CHAT, RESP_CHAT, 0);

    endTimeout();

    return ret;
}


int receiveStatus(struct status *s)
{
    int ret;

    startTimeout();

    ret = msgrcv(clientId, s, RESP_SIZE_STATUS, 0, 0);

    endTimeout();

    return ret;
}


int receiveNotification(struct notificationResponse *notification)
{
    int ret;

    startTimeout();

    ret = msgrcv(clientId, notification, RESP_SIZE_NOTIFICATION, RESP_NOTIFICATION, 0);

    endTimeout();

    return ret;
}


int sendChat(const char *message)
{
    int ret;
    struct chatRequest request;

    request.mtype = REQ_CHAT;
    request.data.clientId = clientId;
    strncpy(request.data.text, message, MAX_MESSAGE);
    request.data.text[MAX_MESSAGE-1] = '\0';

    startTimeout();

    ret = msgsnd(serverId, &request, REQ_SIZE_CHAT, 0);

    endTimeout();

    return ret;
}


int sendName(const char *name)
{
    int ret;

    struct nameRequest request;

    request.mtype = REQ_NAME;
    request.data.clientId = clientId;
    strncpy(request.data.name, name, MAX_NAME);
    request.data.name[MAX_NAME - 1] = '\0';

    startTimeout();

    ret = msgsnd(serverId, &request, REQ_SIZE_NAME, 0);

    endTimeout();

    return ret;
}



// child process calls to handle incoming messages
void messageHandler(void)
{
    int res;
    struct chatResponse chat;

    for (;;) {
        res = receiveChat(&chat);
        if (res == -1) {
            if (errno == EINTR) {
                printf("SERVER TIMEOUT\n");
            }
            break;
        }

        printf("%s: %s", chat.data.name, chat.data.text);
    }
}


// handle notifications from the server
// right now just users leaving and joining
void notificationHandler(void)
{
    int res;
    struct notificationResponse notification;

    for (;;) {
        res = receiveNotification(&notification);
        if (res == -1) {
            if (errno == EINTR) {
                printf("SERVER TIMEOUT\n");
            }
            break;
        }

        if (notification.data.type == USER_JOINED) {
            printf("%s has joined the chat.\n", notification.data.text);
        } else if (notification.data.type == USER_LEFT) {
            printf("%s has left the chat.\n", notification.data.text);
        }
    }
}


int main(int argc, char *argv[])
{
    pid_t childPid;
    int serverId;
    char nameBuf[MAX_NAME];
    char messageBuf[MAX_MESSAGE];
    struct status s;

    if (atexit(cleanup) != 0) {
        errExit("atexit");
    }

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
        errExit("msgget");
    }

    // loop until a valid name is chosen
    for (;;) {

        memset(nameBuf, 0, MAX_NAME);

        if (fgets(nameBuf, MAX_NAME, stdin) == NULL) {
            errExit("fgets");
        }

        nameBuf[MAX_NAME - 1] = '\0';
        nameBuf[MAX_NAME - 2] = '\0';

        if (sendName(nameBuf) == -1) {
            errExit("sendName");
        }

        if (receiveStatus(&s) == -1) {
            errExit("receiveStatus");
        }

        if (s.mtype == RESP_OK) {
            printf("Name accepted\n");
            break;
        } else {
            printf("Invalid name or name is take, choose another\n");
        }
    }

    // fork a handler for server notifications
    switch (childPid = fork()) {

        case -1:
            errExit("fork");

        // child
        case 0:
            notificationHandler();
            _exit(EXIT_FAILURE);

        // parent falls through to create message handler
        default:
            break;
    }

    switch (childPid = fork()) {

        case -1:
            errExit("fork");

        // child
        case 0:
            messageHandler();
            _exit(EXIT_FAILURE);

        // parent falls through to enter message loop
        default:
            break;
    }

    // loop for sending messages now
    for (;;) {

        memset(messageBuf, 0, MAX_MESSAGE);

        printf("> ");

        fflush(stdout);

        if (fgets(messageBuf, MAX_MESSAGE, stdin) == NULL) {
            errExit("fgets");
        }

        if (sendChat(messageBuf) == -1) {
            errExit("sendChat");
        }
    }

    exit(EXIT_SUCCESS);
}
