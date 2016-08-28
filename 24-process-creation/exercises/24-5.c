#include <signal.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"

#define SYNC_SIG SIGUSR1


static void handler(int sig)
{
	return;
}


int main(int argc, char *argv[])
{
	int status;
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
			printf("[%s %ld] Child waiting for parent's signal\n", currTime("%T"), (long)getpid());
			sigemptyset(&emptyMask);
			if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
				errExit("sigsuspend");
			}
			printf("[%s %ld] Child can now get to work\n", currTime("%T"), (long)getpid());
			_exit(EXIT_SUCCESS);

		default:

			printf("[%s %ld] Parent doing some work\n", currTime("%T"), (long)getpid());
			printf("[%s %ld] Parent sending signal to child\n", currTime("%T"), (long)getpid());

			kill(childPid, SYNC_SIG);
			wait(&status);

			printf("Child exited with status: %d\n", status);

			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
