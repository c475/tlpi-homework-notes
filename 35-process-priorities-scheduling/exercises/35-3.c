#define _GNU_SOURCE
#include <sched.h>
#include <sys/times.h>
#include "../../lib/tlpi_hdr.h"


void consume(int length)
{
	struct tms timebuffer;
	clock_t ticksPerSecond;
	double cpuTime;
	int numQuarterSec;
	int numSec;

	ticksPerSecond = sysconf(_SC_CLK_TCK);
	numQuarterSec = 1;
	numSec = 1;

	for (;;) {
		times(&timebuffer);

		cpuTime = (double)timebuffer.tms_utime / (double)ticksPerSecond;

		if (cpuTime >= numQuarterSec * 0.25) {
			printf("[%ld] %lf\n", (long)getpid(), cpuTime);
			numQuarterSec++;
		}

		if (cpuTime >= numSec) {
			printf("[%ld] Yielding\n", (long)getpid());
			numSec++;
			sched_yield();
		}

		if (numSec >= length + 1) {
			printf("[%ld] Exiting\n", (long)getpid());
			break;
		}
	}
}

/*
	This broke my system because I didnt set a resource limit.
	It seems an important system process got starved, resulting in some file system inconsistency
	which lead to the system partition being remounted as read-only, preventing it from booting, something like that.
	Fixable but chose to reinstall after fschk failed to address the issue.
	Maybe the system set the /boot/ partition as read only in /etc/fstab/?

	Program works nicely besides that though.
*/
int main(int argc, char *argv[])
{
	int maxPriority;
	int numCpu;
	struct sched_param param;
	cpu_set_t cpu_set;

	maxPriority = sched_get_priority_max(SCHED_FIFO);
	if (maxPriority == -1) {
		errExit("sched_get_priority_max");
	}

	param.sched_priority = maxPriority;

	if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
		errExit("sched_setscheduler");
	}

	numCpu = sysconf(_SC_NPROCESSORS_ONLN);
	if (numCpu == -1) {
		errExit("sysconf(_SC_NPROCESSORS_ONLN)");
	}

	if (numCpu > 1) {
		CPU_ZERO(&cpu_set);
		CPU_SET(numCpu - 1, &cpu_set);
		if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) == -1) {
			errExit("sched_setaffinity");
		}
	}

	if (fork() == -1) {
		errExit("fork");
	}

	consume(3);

	exit(EXIT_SUCCESS);
}