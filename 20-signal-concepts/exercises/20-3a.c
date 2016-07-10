#include <signal.h>
#include "../../lib/tlpi_hdr.h"

// do nothing sig
void handler(int sig)
{
	return;
}



int main(int argc, char *argv[])
{
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));

	act.sa_handler = handler;
	act.sa_flags = SA_RESETHAND;

	if (sigaction(SIGINT, &act, NULL) == -1) {
		errExit("sigaction()");
	}

	printf("SIGINT is blocked, raising it now\n");

	raise(SIGINT);

	printf("Now raising it again (should terminate)\n");

	raise(SIGINT);

	printf("It did not terminate the process as expected\n");

	exit(EXIT_SUCCESS);
}
