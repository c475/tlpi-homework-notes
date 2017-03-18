#include <sys/time.h>
#include <sys/resource.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int which;
	int prio;
	id_t who;

	if (argc != 4 || strchr("pgu", argv[1][0]) == NULL) {
		usageErr("%s {p|g|u} who priority\n    set priority of:"
			"p=process; g=process group; u=process for user\n", argv[0]);
	}

	// set nice value according to command-line arguments
	if (argv[1][0] == 'p') {
		which = PRIO_PROCESS;

	} else if (argv[1][0] == 'g') {
		which = PRIO_PGRP;

	} else {
		which = PRIO_USER;
	}

	who = getLong(argv[2], 0, "who");
	prio = getInt(argv[3], 0, "prio");

	if (setpriority(which, who, prio) == -1) {
		errExit("setpriority");
	}

	// retrieve nice value to check the change
	errno = 0; // because successful call may return -1

	prio = getpriority(which, who);
	if (prio == -1 && errno != 0) {
		errExit("getpriority");
	}

	printf("Nice value = %d\n", prio);

	exit(EXIT_SUCCESS);
}
