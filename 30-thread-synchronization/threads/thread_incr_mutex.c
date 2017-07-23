#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


static volatile int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;


static void *threadFunc(void *arg)
{
	int loops = *((int *)arg);
	int j;
	int s;

	for (j = 0; j < loops; j++) {
		s = pthread_mutex_lock(&mtx);
		if (s != 0) {
			// errno is set in child process, need to pass "s" val
			errExitEN(s, "pthread_mutex_lock");
		}

		glob++;

		s = pthread_mutex_unlock(&mtx);
		if (s != 0) {
			errExitEN(s, "pthread_mutex_unlock");
		}
	}

	return NULL;
}


int main(int argc, char *argv[])
{
	int s;
	int loops;
	pthread_t t1;
	pthread_t t2;
	void *retval;

	loops = argv[1] ? getInt(argv[1], GN_GT_0, "num-loops") : 1000000;

	s = pthread_create(&t1, NULL, threadFunc, &loops);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t2, NULL, threadFunc, &loops);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_join(t1, &retval);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	s = pthread_join(t2, &retval);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	printf("glob=%d\n", glob);

	exit(EXIT_SUCCESS);
}
