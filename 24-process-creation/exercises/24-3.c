#include <signal.h>
#include <unistd.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	// do some stuff

	// this is the code snippet that would be added where you want a core dump
	if (fork() == 0) {
		kill(getpid(), SIGQUIT);
	}

	// parent can continue
	printf("parent continues\n");

	exit(EXIT_SUCCESS);
}
