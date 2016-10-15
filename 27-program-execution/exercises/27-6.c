#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


void handler(int sig)
{
	printf("Parent got SIGCHLD\n");
}


// not sure what else it would do other than looping on waitpid() forever 
// bc there are no children processes to reap.


// what it really does is reap the child process anyway...
// SIGCHLD is not needed for wait() to work (unless it's handler is set to SIG_IGN)
int main(int argc, char *argv[])
{
	int status;
	pid_t childPid;
	struct sigaction sa;
	sigset_t blockMask, origMask;

	sa.sa_flags = 0;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);

	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGCHLD);

	if (sigprocmask(SIG_SETMASK, &blockMask, &origMask) == -1) {
		errExit("sigprocmask");
	}

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	switch (childPid = fork()) {

		case -1:
			errExit("fork");

		case 0:
			printf("Child exiting.\n");
			_exit(EXIT_SUCCESS);

		default:
			// make sure child exits
			sleep(2);

			while (waitpid(childPid, &status, WNOHANG) == -1) {
				if (errno == ECHILD) {
					printf("Parent has no children...\n");
				}

				printf("Unblocking...");

				if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) {
					errExit("sigprocmask2");
				}
			}

			printf("Child exited with status=%d\n", status);
	}

	exit(EXIT_SUCCESS);
}