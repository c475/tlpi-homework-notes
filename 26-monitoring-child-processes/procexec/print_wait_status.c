#define _GNU_SOURCE
#include <string.h>
#include <sys/wait.h>
#include "print_wait_status.h"
#include "../../lib/tlpi_hdr.h"

/*
	Avoid calling this function from a SIGCHLD handler
	Uses printf, not async-signal-safe etc
*/

void printWaitStatus(const char *msg, int status)
{
	if (msg != NULL) {
		printf("%s", msg);
	}

	if (WIFEXITED(status)) {
		printf("Child exited wuth status=%d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		printf("Child killed by signal %d (%s)", WTERMSIG(status), strsignal(WTERMSIG(status)));

#ifdef WCOREDUMP // not in SUSv3 may be absent on some systems

		if (WCOREDUMP(status)) {
			printf(" (core dumped)");
		}

#endif
	} else if (WIFSTOPPED(status)) {
		printf("Child stopped by signal %d (%s)\n", WSTOPSIG(status), strsignal(WSTOPSIG(status)));

#ifdef WIFCONTINUED // SUSv3 has this, but old linux/unix implentations do not

	} else if (WIFCONTINUED(status)) {
		printf("Child continued\n");

#endif
	} else { // should never happen
		printf("What happened to this child? status=%x\n", (unsigned int)status);
	}
}