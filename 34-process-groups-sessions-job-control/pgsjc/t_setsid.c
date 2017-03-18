// #define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int ragc, char *argv[])
{
	// exit if parent
	if (fork() != 0) {
		_exit(EXIT_SUCCESS);
	}

	if (setsid() == -1) {
		errExit("setsid");
	}

	printf("PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(), (long)getpgrp(), (long)getsid(0));

	if (open("/dev/tty", O_RDWR) == -1) {
		errExit("open /dev/tty");
	}

	exit(EXIT_SUCCESS);
}
