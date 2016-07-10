#include <signal.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	printf("Setting SIGINT handler to SIG_IGN");

	if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
		errExit("signal 1");
	}

	printf("Raising SIGINT signal\n");

	raise(SIGINT);

	printf("Setting SIGINT handler back to SIG_DFL\n");

	if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
		errExit("signal 2");
	}

	printf("Raising SIGINT again. If you don't see anything after this it worked\n");

	raise(SIGINT);

	printf("Didn't terminate as expected after SIGINT...\n");

	exit(EXIT_SUCCESS);
}
