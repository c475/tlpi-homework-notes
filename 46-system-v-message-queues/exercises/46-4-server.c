#include "file_server_header.h"
#include "../../lib/become_daemon.h"


static int serverId;


// dont feel like messing with va_args
static void logToSyslog(const char *msg, const struct requestMsg *req, const char *pathname)
{
    if (req != NULL) {
        syslog(LOG_ERR, msg, req->clientId);
    } else if (pathname != NULL) {
        syslog(LOG_ERR, msg, pathname);
    } else {
        syslog(LOG_ERR, msg);
    }

    closelog();
}


static void exitHandler(void)
{
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }

    if (unlink(SERVER_KEY_FILE) == -1) {
        errExit("unlink");
    }

    closelog();
}


// SIGCHLD handler
static void grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;
    // waitpid() might change errno
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = savedErrno;
}


// SIGTERM and SIGINT handler
static void sigCleanup(int sig)
{
    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        errExit("msgctl");
    }

    if (unlink(SERVER_KEY_FILE) == -1) {
        errExit("unlink");
    }

    closelog();

    if (signal(sig, SIG_DFL) == SIG_ERR) {
        errExit("signal");
    }

    raise(sig);
}


// client timeout trap (SIGALRM)
static void clientTimeoutHandler(int sig)
{
    // do nothing
}


// wrapper for msgsnd() with a timeout feature
// returns 0 on success, -1 on error
int sendMessage(const struct requestMsg *req, const struct responseMsg *resp, size_t size, int timeout)
{
    int ret;
    int savedErrno;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = clientTimeoutHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        return -1;
    }

    if (timeout > 0) {
        alarm(timeout);
    }

    ret = msgsnd(req->clientId, resp, size, 0);

    if (timeout > 0) {
        savedErrno = errno;
        alarm(0);
        errno = savedErrno;
    }

    // client timed out, log to syslog and attempt to delete the client's message queue
    if (ret == -1 && errno == EINTR) {
        logToSyslog("Client timed out, MQID: %d", req, NULL);
        msgctl(req->clientId, IPC_RMID, NULL);
    }

    // Reset the SIGALRM handler? Not sure how I feel about this one.
    // Don't really want to return -1 either since it would imply the client didn't get the message
    signal(SIGALRM, SIG_DFL);

    return ret;
}


static void serveRequest(const struct requestMsg *req)
{
    int fd;
    ssize_t numRead;
    struct responseMsg resp;
    // SIGALRM timeout handler in case the client prematurely terminates

    fd = open(req->pathname, O_RDONLY);

    // open failed, send error response
    if (fd == -1) {
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        sendMessage(req, &resp, strlen(resp.data) + 1, DEFAULT_TIMEOUT);
        logToSyslog("Could not open file: %s\n", NULL, req->pathname);
        _exit(EXIT_FAILURE); // and terminate
    }

    // transmit file contents in messages with type RESP_MT_DATA.
    // we dont diagnose read() and msgsnd() errors since we cant notify the client
    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        if (sendMessage(req, &resp, numRead, DEFAULT_TIMEOUT) == -1) {
            logToSyslog("Unable to send file contents, MQID: %d\n", req, NULL);
            break;
        }
    }

    // send a message of type RESP_MT_END
    resp.mtype = RESP_MT_END;
    sendMessage(req, &resp, 0, DEFAULT_TIMEOUT);
}


int main(int argc, char *argv[])
{
    int fd;
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    struct sigaction sa;

    if (becomeDaemon(0) == -1) {
        errExit("becomeDaemon");
    }

    if (atexit(exitHandler) != 0) {
        errExit("atexit");
    }

    // open syslog connection
    openlog(NULL, LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_USER);

    // create server message queue
    serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverId == -1) {
        errExit("msgget");
    }

    fd = open(SERVER_KEY_FILE, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IWGRP);
    if (fd == -1) {
        errExit("open - server key file");
    }

    if (write(fd, &serverId, sizeof(serverId)) != sizeof(serverId)) {
        errExit("write - server key file");
    }

    if (close(fd) == -1) {
        errExit("close - server key file");
    }

    // establish SIGCHLD handler to reap terminated children
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        errExit("sigaction - SIGCHLD");
    }

    // establish SIGINT and SIGTERM handlers...
    // SIGINT even though this is a daemon now.
    sa.sa_flags = 0;
    sa.sa_handler = sigCleanup;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        errExit("sigaction - SIGTERM");
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        errExit("sigaction - SIGINT");
    }

    // read requests, handle each in separate child process
    for (;;) {
        // get a request
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR) { // interrupted by SIGCHLD?
                continue;
            }

            errMsg("msgrcv"); // some other error
            break; // so terminate loop
        }

        pid = fork(); // create child process
        if (pid == -1) {
            errExit("fork");
            break;
        }

        if (pid == 0) {
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        // parent loops to receive next request
    }

    exit(EXIT_SUCCESS);
}
