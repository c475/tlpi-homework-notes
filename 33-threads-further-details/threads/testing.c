// just for fun
#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static volatile int glob = 0;


static void *threadFunc(void *arg)
{
	int s;

	while (glob < 1000000) {
		s = pthread_mutex_lock(&mtx);
		if (s != 0) {
			errExitEN(s, "pthread_mutex_lock");
		}

		glob++;

		printf("[Thread %d] %d\n", arg, glob);

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
	pthread_t t1;
	pthread_t t2;

	s = pthread_create(&t1, NULL, threadFunc, (void *)1);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t2, NULL, threadFunc, (void *)2);
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

	exit(EXIT_SUCCESS);
}
