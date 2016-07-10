#include <signal.h>
#include "../../lib/tlpi_hdr.h"


static void signalHandler(int sig)
{
	static int count = 0;

	if (sig == SIGINT) {
		count++;
		printf("Caught SIGINT (%d)\n", count);
		return;
	}

	// must be SIGQUIT
	printf("Caught SIGQUIT - that's all folks!\n");
	exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
	if (signal(SIGINT, signalHandler) == SIG_ERR) {
		errExit("signal()");
	}

	if (signal(SIGQUIT, signalHandler) == SIG_ERR) {
		errExit("signal()");
	}

	for (;;) {
		pause();
	}

	exit(EXIT_SUCCESS);
}
