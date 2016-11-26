#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


static void *threadFunc(void *arg)
{
	char *str;

	printf("Other thread about to call strerror()\n");
	str = strerror_NOSAFE(EPERM);
	printf("Other thread: str (%p) = %s\n", str, str);

	return NULL;
}


int main(int argc, char *argv[])
{
	pthread_t t;
	int s;
	char *str;

	str = strerror_NOSAFE(EINVAL);
	printf("main thread called strerror()\n");

	s = pthread_create(&t, NULL, threadFunc, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	// wait for thread to finish
	s = pthread_join(t, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	// thanks to thread specific storage, the main thread has its own buffer
	printf("Main thread: str (%p) = %s\n", str, str);

	exit(EXIT_SUCCESS);
}
