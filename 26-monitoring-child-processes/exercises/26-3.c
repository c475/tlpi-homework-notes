#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


void printWaitStatus2(siginfo_t *info)
{
	// not sure if you can check if core was dumped with waitid() :(
	switch (info->si_code) {

		case CLD_EXITED:
			printf("Child exited with status=%d\n", info->si_status);
			break;

		case CLD_KILLED:
			printf("Child killed by signal %d (%s)", info->si_status, strsignal(info->si_status));
			break;

		case CLD_STOPPED:
			printf("Child stopped by signal %d (%s)\n", info->si_status, strsignal(info->si_status));
			break;

		case CLD_CONTINUED:
			printf("Child continued\n");
			break;

		default:
			printf("What happened to this child? status=%x\n", info->si_status);
	}
}


int main(int argc, char *argv[])
{
	siginfo_t info;

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

				if (waitid(P_ALL, 0, &info, WCONTINUED | WSTOPPED | WEXITED) == -1) {
					errExit("waitid");
				}

				// print status in hex, and as separate decimal bytes
				printf("waitid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
					(long)info.si_pid,
					info.si_status,
					info.si_status >> 8,
					info.si_status & 0xFF
				);

				printWaitStatus2(&info);

				if (info.si_code == CLD_EXITED || info.si_code == CLD_KILLED) {
					exit(EXIT_SUCCESS);
				}
			}

			exit(EXIT_FAILURE);
	}

	exit(EXIT_FAILURE);
}
