#include <time.h>
#include <stdio.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


/*
	It sort of spawns a daemon process
	except it doesnt redirect output to logfiles (redirect stdout)
	or setuid
	or swap stdin to /dev/null/
	or reset signal handlers
	or set umask()
	and various other things
*/
int main(int argc, char *argv[])
{
	int status;
	pid_t childPid;

	childPid = fork();

	if (childPid == -1) {
		errExit("fork");
	}

	// child
	if (childPid == 0) {

		switch (fork()) {

			case -1:
				errExit("fork2");

			case 0:
				// WORK
				sleep(5);

				printf("GRANDCHILD DONE... (deamonish process?)\n");

				// END WORK
				exit(EXIT_SUCCESS);

			default:
				// grandchild gets orphaned
				exit(EXIT_SUCCESS);

		}

	}

	// parent falls thru to here and waits for child
	if (waitpid(childPid, &status, 0) == -1) {
		errExit("waitpid");
	}

	printf("Child exited with status=%d\n", status);

	exit(EXIT_SUCCESS);
}