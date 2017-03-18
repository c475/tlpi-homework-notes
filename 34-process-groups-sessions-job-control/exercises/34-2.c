#include "../../lib/tlpi_hdr.h"


// dirty but demonstrates it
int main(int argc, char *argv[])
{
	pid_t execPid;
	pid_t noexecPid;

	if (argc > 1) {
		// give parent time to change the PGID
		sleep(3);
		printf("[%ld] EXEC PGID AFTER: %ld\n", (long)getpid(), (long)getpgrp());

	} else {
		printf("PARENT PID: %ld\n", (long)getpid());

		execPid = fork();
		if (execPid != 0) {
			noexecPid = fork();
		} else {
			printf("[%ld] EXEC PGID BEFORE: %ld\n", (long)getpid(), (long)getpgrp());
			execlp(argv[0], argv[0], "exec", NULL);
		}

		if (noexecPid == 0) {
			printf("[%ld] NO EXEC PGID BEFORE: %ld\n", (long)getpid(), (long)getpgrp());
			sleep(3);
			printf("[%ld] NO EXEC PGID AFTER: %ld\n", (long)getpid(), (long)getpgrp());
		} else {
			sleep(1);

			if (setpgid(execPid, 0) == -1) {
				printf("Could not change exec PGID for process %ld\n", (long)execPid);
			}

			if (setpgid(noexecPid, 0) == -1) {
				printf("Could not change noexec PGID for process %ld\n", (long)noexecPid);
			}

			sleep(5);
		}

	}

	exit(EXIT_SUCCESS);
}
