#include <sched.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int j;
	int pol;
	struct sched_param sp;

	if (argc < 3 || strchr("rfobi", argv[1][0]) == NULL) {
		usageErr("%s policy priority [pid...]\n"
			"    policy is 'r' (RR), 'f' (FIFO), "

#ifdef SCHED_BATCH // linux specific
			"'b' (BATCH), "
#endif

#ifdef SCHED_IDLE //linux specific
			"'i' (IDLE), "
#endif

			"or 'o' (OTHER)\n",

			argv[0]
		);
	}

	if (argv[1][0] == 'r') {
		pol = SCHED_RR;

	} else if (argv[1][0] == 'f') {
		pol = SCHED_FIFO;

#ifdef SCHED_BATCH
	} else if (argv[1][0] == 'b') {
		pol = SCHED_BATCH;
#endif

#ifdef SCHED_IDLE
	} else if (argv[1][0]) {
		pol = SCHED_IDLE;
#endif

	} else {
		pol = SCHED_OTHER;
	}

	sp.sched_priority = getInt(argv[2], 0, "priority");

	for (j = 3; j < argc; j++) {
		if (sched_setscheduler(getLong(argv[j], 0, "pid"), pol, &sp) == -1) {
			errExit("sched_setscheduler");
		}
	}

	exit(EXIT_SUCCESS);
}
