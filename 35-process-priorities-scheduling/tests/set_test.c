#define _GNU_SOURCE
#include <sched.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int i;
	cpu_set_t set;

	for (;;) {
		for (i = 0; i < 4; i++) {
			CPU_CLR(i-1, &set);
			CPU_SET(i, &set);
			sched_setaffinity(0, sizeof(cpu_set_t), &set);
		}
	}

	exit(EXIT_SUCCESS);
}
