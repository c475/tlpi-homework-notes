#include <signal.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


void handler(int sig)
{
	printf("Got SIGCONT\n");
	exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
	time_t now;
	int elapsed;
	sigset_t oldMask;
	sigset_t newMask;
	struct sigaction sa;

	printf("PID: %ld\n", (long)getpid());

	elapsed = 0;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaddset(&newMask, SIGCONT) == -1) {
		errExit("sigaddset");
	}

	if (sigprocmask(SIG_SETMASK, &newMask, &oldMask) == -1) {
		errExit("sigprocmask1");
	}

	if (sigaction(SIGCONT, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	now = time(NULL);

	printf("You have 10 secs to send this program SIGCONT\n");

	while (elapsed < 10) {
		sleep(1);
		elapsed = (int)time(NULL) - now;
		printf("%d\n", elapsed);
	}

	printf("\nNow, unblocking SIGCONT in 5 seconds...\n");

	now = time(NULL);
	elapsed = 0;

	while (elapsed < 5) {
		sleep(1);
		elapsed = (int)time(NULL) - now;
		printf("%d\n", elapsed);
	}

	printf("\nUnblocking now...\n");

	if (sigprocmask(SIG_SETMASK, &oldMask, NULL) == -1) {
		errExit("sigprocmask2");
	}

	printf("Didn't catch SIGCONT, did you send it?\n");

	exit(EXIT_FAILURE);
}
