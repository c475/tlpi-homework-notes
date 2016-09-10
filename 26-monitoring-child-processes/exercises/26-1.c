#include "../../lib/tlpi_hdr.h"


// this actually returns the PID for "upstart" because I'm on ubuntu...
// which is strange bc it's 16.04 and uses systemd now

/*

	$ ps -p 1 -o comm=
	systemd

*/
int main(int argc, char *argv[])
{
	switch (fork()) {

		case -1:
			errExit("fork");

		case 0:
			sleep(3);
			printf("NEW PARENT: %ld\n", (long)getppid());
			_exit(EXIT_SUCCESS);

		default:
			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
