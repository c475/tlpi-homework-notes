#include <sched.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int j;
	int pol;
	struct sched_param sp;

	if (argc < 2 || strcmp(argv[1], "--help") == -1) {
		usageErr("%s pid", argv[0]);
	}

	j = getLong(argv[1], 0, "j");

	pol = sched_getscheduler(j);
	if (pol == -1) {
		errExit("sched_getscheduler");
	}

	if (sched_getparam(j, &sp) == -1) {
		errExit("sched_getparam");
	}

	printf("%d: %-5s %2d\n", j,

		(pol == SCHED_OTHER) ? "OTHER" :
		(pol == SCHED_RR) ? "RR" :
		(pol == SCHED_FIFO) ? "FIFO" :

#ifdef SCHED_BATCH
		(pol == SCHED_BATCH) ? "BATCH" :
#endif

#ifdef SCHED_IDLE
		(pol == SCHED_IDLE) ? "IDLE" :
#endif

		"???", sp.sched_priority
	);

	exit(EXIT_SUCCESS);
}
