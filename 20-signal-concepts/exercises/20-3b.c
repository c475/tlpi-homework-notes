#include <signal.h>
#include "../../lib/tlpi_hdr.h"


static int count = 1;


void handler(int sig)
{
	printf("Called %d times\n", count++);
	// press CTRL+c to increment, CTRL+z to stop
	// demonstrating that SA_NODEFER option does not add the signal to the process mask
	pause();
}


int main(int argc, char *argv[])
{
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));

	act.sa_handler = handler;
	act.sa_flags = SA_NODEFER;

	if (sigaction(SIGINT, &act, NULL) == -1) {
		errExit("sigaction()");
	}

	raise(SIGINT);

	exit(EXIT_SUCCESS);
}
