#include <pthread.h>
#include <sys/wait.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


// The SIGCHLD signal will be delivered to an arbitrary thread
// Proof is SIGCHLD being delivered to a thread other than thread "1"
// in fact it seems like "1" never gets the signal
void *threadFunc(void *arg)
{
	int status;

	// this thread creates the child
	if (strcmp((char *)arg, "1") == 0) {
		// parent
		if (fork() != 0) {
			// make sure there is a child process...
			sleep(1);
			if (wait(&status) != -1) {
				printf("Thread %s got SIGCHLD\n", (char *)arg);
			}

			return NULL;

		// child sleeps and returns
		} else {
			sleep(3);
			_exit(0);
		}


	// just wait for SIGCHLD instead
	} else {
		// make sure there is a child process to wait on...
		sleep(2);

		if (wait(&status) != -1) {
			printf("Thread %s got SIGCHLD\n", (char *)arg);
		}

		return NULL;
	}

	return NULL;
}


// run this enough times to find out
int main(int argc, char *argv[])
{
	int s;
	pthread_t t1;
	pthread_t t2;
	pthread_t t3;
	pthread_t t4;

	s = pthread_create(&t1, NULL, threadFunc, "1");
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t2, NULL, threadFunc, "2");
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t3, NULL, threadFunc, "3");
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t4, NULL, threadFunc, "4");
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

	s = pthread_join(t3, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	s = pthread_join(t4, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	// shouldnt get here
	exit(EXIT_SUCCESS);
}