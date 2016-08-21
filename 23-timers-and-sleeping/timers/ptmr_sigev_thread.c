#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "../../lib/tlpi_hdr.h"
#include "itimerspec_from_str.h"


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// number of timer expirations
static int expireCnt = 0;


static void threadFunc(union sigval sv)
{
	timer_t *tidptr;
	int s;

	tidptr = sv.sival_ptr;

	printf("[%s] Thread notify\n", currTime("%T"));
	printf("    timer ID=%ld\n", (long)*tidptr);
	printf("    timer_getoverrun()=%d\n", timer_getoverrun(*tidptr));

	// increment counter variable shared with main thread
	// and signal condition variable to notify main thread of the change

	s = pthread_mutex_lock(&mtx);
	if (s != 0) {
		errExitEN(s, "pthread_mutex_lock");
	}

	expireCnt += 1 + timer_getoverrun(*tidptr);

	s = pthread_mutex_unlock(&mtx);
	if (s != 0) {
		errExitEN(s, "pthread_mutex_unlock");
	}

	s = pthread_cond_signal(&cond);
	if (s != 0) {
		errExitEN(s, "pthread_cond_signal");
	}
}


int main(int argc, char *argv[])
{
	struct sigevent sev;
	struct itimerspec ts;
	timer_t *tidlist;
	int s, j;

	if (argc < 2) {
		
	}
}