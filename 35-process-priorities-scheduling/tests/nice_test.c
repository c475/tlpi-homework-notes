#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>

/*
	nice = 0

		real	0m0.276s
		user	0m0.276s
		sys	    0m0.000s

	nice = 19

		real	0m0.270s
		user	0m0.268s
		sys		0m0.000s

	nice = -18


*/
int main(int argc, char *argv[])
{
	int i;
	int x;
	long nice_val = strtol(argv[1], NULL, 10);
	if (setpriority(0, PRIO_PROCESS, nice_val) == -1) {
		printf("setpriotiry failed");
		getchar();
	}

	for (i = 0; i < 100000000; i++) {
		x += i % 3;
	}

	return 0;
}
