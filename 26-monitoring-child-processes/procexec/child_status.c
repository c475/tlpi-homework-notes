#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


void printWaitStatus2(siginfo_t *info)
{

}





int main(int argc, char *argv[])
{
	int status;
	pid_t childPid;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		usageErr("%s [exit-status]\n", argv[0]);
	}

	switch (fork()) {

		case -1:
			errExit("fork");

		case 0:
			printf("Child started with PID=%ld\n", (long)getpid());

			// exit status supplied on command line
			if (argc > 1) {
				exit(getInt(argv[1], 0, "exit-status"));

			// otherwise, wait for signals
			} else {
				for (;;) {
					pause();
				}
			}

			// not reached, but good practice
			exit(EXIT_FAILURE);

		default:

			for (;;) {

				childPid = waitpid(
					-1,
					&status, 
					WUNTRACED
#ifdef WCONTINUED
					| WCONTINUED 
#endif
				);

				if (childPid == -1) {
					errExit("waitpid");
				}

				// print status in hex, and as separate decimal bytes
				printf("waitpid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
					(long)childPid,
					(unsigned int)status,
					status >> 8,
					status & 0xFF
				);

				printWaitStatus(NULL, status);

				if (WIFEXITED(status) || WIFSIGNALED(status)) {
					exit(EXIT_SUCCESS);
				}
			}

			exit(EXIT_FAILURE);
	}

	exit(EXIT_FAILURE);
}
