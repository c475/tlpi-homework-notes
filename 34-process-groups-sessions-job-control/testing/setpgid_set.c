#include <unistd.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int pgid;
	int pid;

	if (argc == 1) {
		usageErr("%s PGID\n", argv[0]);
	}

	pgid = getInt(argv[1], GN_NONNEG, NULL);

	if (argc >= 3) {
		pid = getInt(argv[2], GN_NONNEG, NULL);
	} else {
		pid = (int)getpid();
	}

	if (pgid == -1) {
		errExit("getInt");
	}

	printf("PID to change is %d\n", pid);

	printf("Setting PGID to %d\n", pgid);

	if (setpgid(pid, (pid_t)pgid) == -1) {
		printf("failed...\n");
	}

	printf("Process PGID is %d\n", (int)getpgrp());

	exit(EXIT_SUCCESS);
}
