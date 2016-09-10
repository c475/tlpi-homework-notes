#include <signal.h>
#include <libgen.h>
#include "../../lib/tlpi_hdr.h"

#define CMD_SIZE 200


int main(int argc, char *argv[])
{
	char cmd[CMD_SIZE];
	pid_t childPid;

	// because multiple processes and user space buffering dont go together
	setbuf(stdout, NULL);

	printf("Parent PID=%ld\n", (long)getpid());

	switch (childPid = fork()) {

		case -1:
			errExit("fork");

		// child exits immediately to become zombie
		case 0:
			printf("Child (PID=%ld) exiting\n", (long)getpid());
			_exit(EXIT_SUCCESS);

		default:
			// give child chance to start and exit
			sleep(3);

			snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
			printf("cmd: %s\n", cmd);
			// view zombie child
			system(cmd);

			// now send the "sure kill" signal to the zombie
			if (kill(childPid, SIGKILL) == -1) {
				errMsg("kill");
			}

			// give child chance to react to signal
			sleep(3);

			printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)childPid);
			system(cmd);

			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
