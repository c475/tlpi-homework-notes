#include <sys/wait.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int numDead;
	pid_t childPid;
	int j;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s sleep-time...\n", argv[0]);
	}

	setbuf(stdout, NULL);

	for (j = 1; j < argc; j++) {
		switch (fork()) {

			case -1:
				errExit("fork");

			case 0:
				printf("[%s] child %d started with PID %ld, sleeping %s seconds\n",
					currTime("%T"), j, (long)getpid(), argv[j]
				);

				sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
				_exit(EXIT_SUCCESS);

			default:
				break;
		}
	}

	numDead = 0;
	for (;;) {
		childPid = wait(NULL);
		if (childPid == -1) {
			if (errno == ECHILD) {
				printf("No more children, exiting\n");
				exit(EXIT_SUCCESS);
			} else {
				errExit("wait");
			}
		}

		numDead++;

		printf("[%s] wait() returned child PID %ld (numDead=%d)\n",
			currTime("%T"), (long)childPid, numDead
		);
	}

	printf("wtf?\n");
	exit(EXIT_FAILURE);
}
