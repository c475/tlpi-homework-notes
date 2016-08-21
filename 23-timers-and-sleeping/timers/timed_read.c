#include <signal.h>
#include "../../lib/tlpi_hdr.h"

#define BUF_SIZE 200


static void handler(int sig)
{
	printf("caught signal...\n");
}


// "theoretical" race condition here.... between the end of read() and the alarm(0) call.
// there is a solution with longjmp, check it out later
int main(int argc, char *argv[])
{
	struct sigaction sa;
	char buf[BUF_SIZE];
	ssize_t numRead;
	int savedErrno;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		usageErr("%s [num-secs [restart-flag]]\n", argv[0]);
	}

	// set up handler for SIGALRM
	sa.sa_flags = argc > 2 ? SA_RESTART : 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler;

	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	alarm(argc > 1 ? getInt(argv[1], GN_NONNEG, "num-secs") : 10);

	numRead = read(STDIN_FILENO, buf, BUF_SIZE);

	savedErrno = errno; // in case alarm() changes it
	alarm(0); // ensure timer is turned off
	errno = savedErrno;

	// determine result of read()
	if (numRead == -1) {
		if (errno == EINTR) {
			printf("Read timed out\n");
		} else {
			errMsg("read");
		}
	} else {
		printf("successful read (%ld bytes): %s", (long)numRead, buf);
	}

	exit(EXIT_SUCCESS);
}
