#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define NOTIFY_SIG SIGUSR1


static void handler(int sig)
{
    // just interrupt sigsuspend()
}


int main(int argc, char *argv[])
{
    struct sigevent sev;
    mqd_t mqd;
    struct mq_attr attr;
    void *buffer;
    ssize_t numRead;
    sigset_t blockMask;
    sigset_t emptyMask;
    struct sigaction sa;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s mq-name\n", argv[0]);
    }

    // open the queue
    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t)-1) {
        errExit("mq_open");
    }

    // get the message size of the queue
    if (mq_getattr(mqd, &attr) == -1) {
        errExit("mq_getattr");
    }

    // ... so you can make a buffer of the correct size
    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        errExit("malloc");
    }

    // block the notification signal while we get things set up
    sigemptyset(&blockMask);
    sigaddset(&blockMask, NOTIFY_SIG);
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
        errExit("sigprocmask");
    }

    // set up the notification handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    // set up the actual notification on the message queue
    sev.sigev_notify = SIGEV_SIGNAL; // get notified via signal
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1) {
        errExit("mq_notify");
    }

    sigemptyset(&emptyMask);

    for (;;) {
        // suspend the process with the empty signal mask so we can get the previously blocked notification signal
        sigsuspend(&emptyMask); // wait for notification signal

        // need to re-register the event since it should now be removed
        // it is important that this step happens BEFORE the mq_receive()
        // otherwise a message can be put on the queue before this is called, leaving the program hanging at sigsuspend() forever
        if (mq_notify(mqd, &sev) == -1) {
            errExit("mq_notify");
        }

        // there has to be a message on the queue now (remember O_NONBLOCK is being used)
        // consumes all messages, then fails on EAGAIN, continuing the loop
        while ((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0) {
            printf("Read %ld bytes\n", (long)numRead);
        }

        if (errno != EAGAIN) { // unexpected error
            errExit("mq_receive");
        }
    }

    exit(EXIT_SUCCESS);
}
