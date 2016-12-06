#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int sentSignals = 0;


static void *threadFunc(void *arg)
{
	int s;
	sigset_t blockMask;
	sigset_t pending;

	if (sigfillset(&blockMask) == -1) {
		errExit("sigfillset");
	}

	s = pthread_sigmask(SIG_BLOCK, &blockMask, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_sigmask");
	}

	s = pthread_mutex_lock(&mtx);
	if (s != 0) {
		errExitEN(s, "pthread_mutex_lock");
	}

	// wait for main thread to send signals
	while (sentSignals == 0) {
		s = pthread_cond_wait(&cond, &mtx);
		if (s != 0) {
			errExitEN(s, "pthread_cond_wait");
		}
	}

	if (sigpending(&pending) == -1) {
		errExit("sigpending");
	}

	printSigset(stdout, (char *)arg, &pending);

	s = pthread_mutex_unlock(&mtx);
	if (s != 0) {
		errExitEN(s, "pthread_mutex_unlock");
	}

	return NULL;
}


int main(int argc, char *argv[])
{
	int s;
	pthread_t t1;
	pthread_t t2;

	s = pthread_create(&t1, NULL, threadFunc, "Thread 1: ");
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_create(&t2, NULL, threadFunc, "Thread 2: ");
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	// give threads time to block signals and wait on condition variable
	// hate doing this
	sleep(2);


	// send different signals to each thread
	s = pthread_kill(t1, SIGINT);
	if (s != 0) {
		errExitEN(s, "pthread_kill");
	}

	s = pthread_kill(t2, SIGTERM);
	if (s != 0) {
		errExitEN(s, "pthread_kill");
	}

	// wake threads up and let them print their pending signals
	sentSignals = 1;
	s = pthread_cond_broadcast(&cond);
	if (s != 0) {
		errExitEN(s, "pthread_cond_broadcast");
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