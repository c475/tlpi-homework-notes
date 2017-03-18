#include <sched.h>
#include "../../lib/tlpi_hdr.h"


int set_policy(int policy, int priority)
{
	int min;
	int max;
	struct sched_param param;

	min = sched_get_priority_min(policy);
	max = sched_get_priority_max(policy);

	if (min == -1 || max == -1) {
		return -1;
	}

	if (sched_getparam(0, &param) == -1) {
		return -1;
	}

	param.sched_priority += priority;

	if (param.sched_priority < min) {
		param.sched_priority = min;
	} else if (param.sched_priority > max) {
		param.sched_priority = max;
	}

	if (sched_setscheduler(0, policy, &param) == -1) {
		return -1;
	}

	return 0;
}


/*
	sched_setscheduler() does not require root on Ubuntu 16.04 apparently
*/
int main(int argc, char *argv[])
{
	uid_t uid;
	int priority;
	int policy;

	if (argc < 4) {
		usageErr("%s {r|f} priority command [arg]...\n", argv[0]);
	}

	if (argv[1][0] == 'r') {
		policy = SCHED_RR;
	} else if (argv[1][0] == 'f') {
		policy = SCHED_FIFO;
	} else {
		usageErr("Unknown priotity '%c': %s {r|f} priority command [arg]...\n", argv[1][0], argv[0]);
	}

	priority = getLong(argv[2], 0, "priority");

	if (set_policy(policy, priority) == -1) {
		errExit("set_policy");
	}

	uid = userIdFromName("nobody");
	if (uid == -1) {
		errExit("Can't change user to 'nobody': userIdFromName()");
	}

	setuid(uid);

	execvp(argv[3], argv + 3);

	fprintf(stderr, "Failed to exec\n");

	exit(EXIT_FAILURE);
}
