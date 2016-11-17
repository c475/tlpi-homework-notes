#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


struct thread_identifier {
	int id;
	int loops;
};

// prevent compiler optimizations of arithmetic operations
static volatile int glob = 0;


static void *threadFunc(void *arg)
{
	struct thread_identifier *tid = (struct thread_identifier *)arg;
	int j;

	for (j = 0; j < tid->loops; j++) {
		// it may seem like this would work but have to remember
		// that ++ is NOT a single CPU instruction
		// so if a time slice expires somewhere before the final instruction
		// you're gonna have problems
		glob++;

		printf("[%d] glob=%d\n", tid->id, glob);
	}

	return NULL;
}


/*
	Redirect stdout to a file

	$ ./30-1 >> output.txt

*/
int main(int argc, char *argv[])
{
	struct thread_identifier ti1, ti2;
	pthread_t t1, t2;
	int loops, s;

	loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

	ti1.loops = loops;
	ti1.id = 1;
	ti2.loops = loops;
	ti2.id = 2;

	s = pthread_create(&t1, NULL, threadFunc, &ti1);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t2, NULL, threadFunc, &ti2);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_join(t1, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	s = pthread_join(t2, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	printf("glob = %d\n", glob);

	exit(EXIT_SUCCESS);
}
