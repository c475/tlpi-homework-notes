#include <signal.h>
#include "../../lib/tlpi_hdr.h"

#define SYNC_SIG SIGUSR1


static void handler(int sig)
{
	return;
}


int main(int argc, char *argv[])
{
	pid_t childPid;
	sigset_t blockMask, origMask, emptyMask;
	struct sigaction sa;

	// disable userspace buffering
	setbuf(stdout, NULL);

	sigemptyset(&blockMask);
	// block sync signal
	sigaddset(&blockMask, SYNC_SIG);
	if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) {
		errExit("sigprocmask");
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	if (sigaction(SYNC_SIG, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	switch (childPid = fork()) {

		case -1:
			errExit("fork");

		case 0:
			// child does some required action here
			printf("[%s %ld] Child started - doing work\n", currTime("%T"), (long)getpid());
			sleep(2);

			printf("[%s %ld] Child about to signal parent\n", currTime("%T"), (long)getpid());

			if (kill(getppid(), SYNC_SIG) == -1) {
				errExit("kill");
			}

			// now child can do other things

			_exit(EXIT_SUCCESS);

		default:
			// parent can do some work here, 
			// then waits for child to complete required action

			printf("[%s %ld] Parent about to wait for signal\n", currTime("%T"), (long)getpid());
			sigemptyset(&emptyMask);
			if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
				errExit("sigsuspend");
			}

			printf("[%s %ld] Parent got signal\n", currTime("%T"), (long)getpid());

			// If required, return signal mask to original state

			if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) {
				errExit("sigprocmask");
			}

			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
