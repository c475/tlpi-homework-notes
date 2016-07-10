#define _GNU_SOURCE
#include <signal.h>
#include "../../lib/tlpi_hdr.h"

static int sigCnt[NSIG]; // counts deliveries of each signal
static volatile sig_atomic_t gotSigint = 0; // set nonzero if SIGINT is delivered


static void handler(int sig)
{
	if (sig == SIGINT) {
		gotSigint = 1;
	} else {
		sigCnt[sig]++;
	}
}


int main(int argc, char *argv[])
{
	int n;
	int numSecs;
	sigset_t pendingMask;
	sigset_t blockingMask;
	sigset_t emptyMask;

	printf("%s: PID is %ld\n", argv[0], (long)getpid());

	// same handler for all signals
	for (n = 1; n < NSIG; n++) {
		// ignore errors
		(void)signal(n, handler);
	}

	// if a sleep time is specified, block all signals temporarily
	// while the other process sends signals. Then display the pending signals,
	// then unblock all signals

	if (argc > 1) {
		numSecs = getLong(argv[1], GN_GT_0, NULL);

		sigfillset(&blockingMask);
		if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1) {
			errExit("sigprocmask");
		}

		printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
		sleep(numSecs);

		if (sigpending(&pendingMask) == -1) {
			errExit("sigpending");
		}

		printf("%s: pending signals are: \n", argv[0]);
		printSigset(stdout, "\t\t", &pendingMask);

		// unblock all signals
		sigemptyset(&emptyMask);
		if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1) {
			errExit("sigprocmask()");
		}
	}

	while (!gotSigint) {
		continue;
	}

	// display number of signals received
	for (n = 1; n < NSIG; n++) {
		if (sigCnt[n] != 0) {
			printf("%s: signal %d caught %d time%s", argv[0], n, sigCnt[n], (sigCnt[n] == 1) ? "" : "s");
		}
	}

	exit(EXIT_SUCCESS);
}
