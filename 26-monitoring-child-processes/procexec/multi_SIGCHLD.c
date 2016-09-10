#include <signal.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


static volatile int numLiveChildren = 0;


static void sigchldHandler(int sig)
{
	int status;
	int savedErrno;
	pid_t childPid;

	// unsafe things happening in this function, etc

	// in case handler modifies errno
	savedErrno = errno;

	printf("[%s] handler: caught SIGCHLD\n", currTime("%T"));

	while ((childPid = waitpid(-1, &status, WNOHANG)) > 0) {
		printf("[%s] handler: Reaped child %ld - ", currTime("%T"), (long)childPid);
		printWaitStatus(NULL, status);
		numLiveChildren--;
	}

	if (childPid == -1 && errno != ECHILD) {
		errMsg("waitpid");
	}

	// artificially lengthen execution of handler
	sleep(5);

	printf("[%s] handler: returning\n", currTime("%T"));

	errno = savedErrno;
}


int main(int argc, char *argv[])
{
	int j;
	int sigCnt;
	sigset_t blockMask;
	sigset_t emptyMask;
	struct sigaction sa;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s child-sleep-time...\n", argv[0]);
	}

	// disable stdout buffering
	setbuf(stdout, NULL);

	sigCnt = 0;
	numLiveChildren = argc - 1;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigchldHandler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	// block SIGCHLD to prevent its delivery if a child terminates
	// before the parent commences the sigsuspend() loop below

	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGCHLD);
	if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1) {
		errExit("sigprocmask");
	}

	for (j = 1; j < argc; j++) {

		switch (fork()) {

			case -1:
				errExit("fork");

			case 0:
				sleep(getInt(argv[j], GN_NONNEG, "child-sleep-time"));
				printf("[%s] Child %d (PID=%ld) exiting\n", currTime("%T"), j, (long)getpid());
				_exit(EXIT_SUCCESS);

			// parent just loops to create next child
			default:
				break;
		}
	}

	sigemptyset(&emptyMask);
	while (numLiveChildren > 0) {
		if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
			errExit("sigsuspend");
		}

		sigCnt++;
	}

	printf("[%s] All %d children have terminated; SIGCHLD was caught %d times\n",
		currTime("%T"),
		argc - 1,
		sigCnt
	);

	exit(EXIT_SUCCESS);
}
