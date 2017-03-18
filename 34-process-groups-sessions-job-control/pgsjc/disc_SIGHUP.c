#define _GNU_SOURCE // strsignal()
#include <string.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


static void handler(int sig)
{
	printf("PID %ld: caught signal %2d (%s)\n", (long)getpid(), sig, strsignal(sig));
	// write(1, "SIGHUP\n", 7);
}


int main(int argc, char *argv[])
{
	int j;
	pid_t parentPid;
	pid_t childPid;
	struct sigaction sa;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s {d|s}... [ > sig.log 2>&1 ]\n", argv[0]);
	}

	// make stdout unbuffered
	setbuf(stdout, NULL);

	parentPid = getpid();
	printf("PID of parent process is:       %ld\n", (long)parentPid);
	printf("Foreground process group ID is: %ld\n", (long)tcgetpgrp(STDIN_FILENO));

	for (j = 1; j < argc; j++) {
		childPid = fork();
		if (childPid == -1) {
			errExit("fork");
		}

		// is child process
		if (childPid == 0) {
			// move to different process group
			if (argv[j][0] == 'd') {
				if (setpgid(0, 0) == -1) {
					errExit("setpgid");
				}
			}

			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = handler;
			if (sigaction(SIGHUP, &sa, NULL) == -1) {
				errExit("sigaction");
			}
			// child exits loop so it doesnt spawn more processes
			break;
		}
	}

	// all pocesses fall through here

	// ensure all processes terminate no matter what
	alarm(60);

	printf("PID=%ld PGID=%ld\n", (long)getpid(), (long)getpgrp());
	for (;;) {
		// wait for signals
		pause();
	}

	exit(EXIT_FAILURE);
}
