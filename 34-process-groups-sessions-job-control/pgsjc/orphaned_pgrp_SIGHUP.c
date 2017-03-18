#define _GNU_SOURCE // strsignal()
#include <string.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


static void handler(int sig)
{
	printf("PID=%ld: caught signal %d (%s)\n", (long)getpid(), sig, strsignal(sig));
}


int main(int argc, char *argv[])
{
	int j;
	struct sigaction sa;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s {s|p} ...\n", argv[0]);
	}

	setbuf(stdout, NULL);

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		errExit("sigaction");
	}
	if (sigaction(SIGCONT, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	printf("parent: PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n",
		(long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(0));

	// create one child for seahc command line argument
	for (j = 1; j < argc; j++) {
		switch (fork()) {

			case -1:
				errExit("fork");

			// child
			case 0:
				printf("child: PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n",
					(long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(0));

				// stop via signal
				if (argv[j][0] == 's') {
					printf("PID=%ld stopping\n", (long)getpid());
					raise(SIGSTOP);

				// wait for the signal
				} else {
					// die no matter what
					alarm(60);
					printf("PID=%ld pausing\n", (long)getpid());
					pause();
				}

				_exit(EXIT_SUCCESS);

			// parent carries on around loop
			default:
				break;
		}
	}

	// parent falls through here
	sleep(3);
	printf("parent exiting\n");
	exit(EXIT_SUCCESS); // orphan the children and their group
}
