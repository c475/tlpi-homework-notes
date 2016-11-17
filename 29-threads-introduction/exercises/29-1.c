#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


/*
I'm expecting either:

	1. Thread deadlocks.
	2. pthread_join() is smart enough see if the calling thread is the thread being joined,
		and will refuse to do it (more likely)
*/

/*
	I was mostly right, but it returns an error code set to EDEADLK/EDEADLOCK
*/
static void *threadFunc(void *arg)
{
	int s;
	printf("Joining inside thread\n");
	// s = 
	pthread_join(pthread_self(), NULL);
	// if (s != 0) {
	// 	errExitEN(s, "pthread_join");
	// }
	printf("Join success\n");
	pthread_exit(arg);
}


int main(int argc, char *argv[])
{
	pthread_t tid;
	int s;

	s = pthread_create(&tid, NULL, threadFunc, "test");
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	printf("Exiting main in 1 second\n");

	sleep(1);

	exit(EXIT_SUCCESS);
}
