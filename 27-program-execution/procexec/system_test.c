#include <unistd.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int status;
	struct sigaction ignoreChild;
	struct sigaction defaultChild;

	ignoreChild.sa_handler = SIG_IGN;
	ignoreChild.sa_flags = 0;
	sigemptyset(&ignoreChild.sa_mask);

	if (sigaction(SIGCHLD, &ignoreChild, &defaultChild) == -1) {
		errExit("sigaction");
	}

	// waitpid() fails with errno != EINTR, can't return actual status
	status = system("ls");

	printf("Status when SIGCHLD == SIG_IGN: %d\n", status);

	ignoreChild.sa_handler = SIG_DFL;

	if (sigaction(SIGCHLD, &defaultChild, &ignoreChild) == -1) {
		errExit("sigaction2");
	}

	// can now properly return status
	status = system("ls");

	printf("Status when SIGCHLD == SIG_DFL: %d\n", status);

	exit(EXIT_SUCCESS);
}
