#define _POSIX_C_SOURCE 199309
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"

/*
	$ ./23-2 60 &
	$ while true; do kill -INT pid; done
	
	...

	Remaining: 1901.148233999
	Slept for: 671.717259 secs
	Remaining: 1901.148277584
	Slept for: 671.717274 secs
	Remaining: 1901.148322271
	Slept for: 671.717288 secs
	Remaining: 1901.148366107
	Slept for: 671.717303 secs
	Remaining: 1901.148409897
	Slept for: 671.717318 secs
	Remaining: 1901.148454686

	...

	On my computer the requested amount of time is constantly rounded up because of this:

       If the interval specified in req is not an exact multiple of the
       granularity underlying clock (see time(7)), then the interval will be
       rounded up to the next multiple.

    So... the process will never stop sleeping.

    Using clock_nanosleep():




*/



static void sigintHandler(int sig)
{
	return;
}


int main(int argc, char *argv[])
{
	struct timeval start;
	struct timeval finish;
	struct timespec request;
	struct timespec remain;
	struct sigaction sa;
	int s;
	long nsec;

	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s secs nanosecs\n", argv[0]);
	}

	printf("PID: %ld\n", (long)getpid());

	if (clock_gettime(CLOCK_REALTIME, &request) == -1) {
		errExit("clock_gettime");
	}

	request.tv_sec += getLong(argv[1], 0, "secs");
	request.tv_nsec += getLong(argv[2], 0, "nanosecs");

	// allow SIGINT handler to interrupt nanosleep
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	if (gettimeofday(&start, NULL) == -1) {
		errExit("gettimeofday");
	}

	for (;;) {
		// will not use remain if TIMER_ABSTIME flag is used
		s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, NULL);
		if (s == -1 && errno != EINTR) {
			errExit("nanosleep");
		}

		if (gettimeofday(&finish, NULL) == -1) {
			errExit("gettimeofday");
		}

		printf("Slept for: %9.6f secs\n",
			finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0
		);

		// nanosleep completed
		if (s == 0) {
			break;
		}

		if (clock_gettime(CLOCK_REALTIME, &remain) == -1) {
			errExit("clock_gettime2");
		}

		remain.tv_nsec /= 10;
		nsec = (request.tv_nsec / 10) - remain.tv_nsec;
		if (nsec < 0) {
			nsec += 100000000;
		}

		printf("Remaining: %2ld.%8ld\n", (long)request.tv_sec - (long)remain.tv_sec, nsec);
	}

	printf("Sleep complete\n");
	exit(EXIT_SUCCESS);
}
