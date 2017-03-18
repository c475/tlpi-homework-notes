#include <signal.h>
#include "../../lib/tlpi_hdr.h"


static void tstpHandler(int sig)
{
	sigset_t tstpMask;
	sigset_t prevMask;
	int savedErrno;
	struct sigaction sa;

	// in case handler changes errno
	savedErrno = errno;

	// unsafe blah blah
	printf("Caught SIGTSTP\n");

	// set SIGTSTP to default handler
	if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
		errExit("signal");
	}

	// queue up SIGTSTP again
	raise(SIGTSTP);

	sigemptyset(&tstpMask);
	sigaddset(&tstpMask, SIGTSTP);
	if (sigprocmask(SIG_UNBLOCK, &tstpMask, &prevMask) == -1) {
		errExit("sigprocmask");
	}

	// execution resumes here after SIGCONT
	// reblock SIGTSTP
	if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) {
		errExit("sigprocmask");
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = tstpHandler;
	if (sigaction(SIGTSTP, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	printf("Exiting SIGTSTP handler\n");
	errno = savedErrno;
}


int main(int argc, char *argv[])
{
	struct sigaction sa;

	// only establish handler for SIGTSTP if it is not being ignored
	if (sigaction(SIGTSTP, NULL, &sa) == -1) {
		errExit("sigaction");
	}

	if (sa.sa_handler != SIG_IGN) {
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = tstpHandler;
		if (sigaction(SIGTSTP, &sa, NULL) == -1) {
			errExit("sigaction");
		}
	}

	for (;;) {
		pause();
		printf("Main\n");
	}

	exit(EXIT_FAILURE);
}
