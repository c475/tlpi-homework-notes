#include <sys/time.h>
#include <sys/resource.h>
#include "../../lib/tlpi_hdr.h"


int _nice(int inc)
{
	int current_priority;

	errno = 0;
	current_priority = getpriority(PRIO_PROCESS, 0);
	if (current_priority == -1 && errno != 0) {
		return -1;
	}

	current_priority += inc;

	if (current_priority < -20) {
		current_priority = -20;
	} else if (current_priority > 19) {
		current_priority = 19;
	}

	if (setpriority(PRIO_PROCESS, 0, current_priority) == -1) {
		return -1;
	}

	return current_priority;
}


int main(int argc, char *argv[])
{
	int prio;

	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s nice-value cmd [args...]\n", argv[0]);
	}

	if (strcmp(argv[1], "--version") == 0) {
		usageErr("version: %s\n", "1.0");
	}

	if (argc > 1) {
		prio = getLong(argv[1], 0, "prio");
	} else {
		prio = 10;
	}

	if (_nice(prio) == -1 && errno != 0) {
		errExit("_nice");
	}

	execvp(argv[2], argv + 2);

	exit(EXIT_FAILURE);
}
