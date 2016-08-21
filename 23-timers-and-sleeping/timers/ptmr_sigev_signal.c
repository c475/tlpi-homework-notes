#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <time.h>
#include "itimerspec_from_str.h"
#include "../../lib/tlpi_hdr.h"

#define TIMER_SIG SIGRTMAX


static void handler(int sig, siginfo_t *si, void *uc)
{
	// represents timer object
	timer_t *tidptr;

	// you can share the timer object with the handler this way
	tidptr = si->si_value.sival_ptr;

	printf("[%s] Got signal %d\n", currTime("%T"), sig);
	printf("    *sival_ptr         = %ld\n", (long)*tidptr);
	printf("    timer_gotoverrun() = %d\n", timer_getoverrun(*tidptr));
}


int main(int argc, char *argv[])
{
	// structure describing the timer (length and interval)
	struct itimerspec ts;

	// signal handler
	struct sigaction sa;

	// describe the way to handle a signal on POSIX timer expiration
	struct sigevent sev;

	// make a list of timers
	timer_t *tidlist;

	int j;

	if (argc < 2) {
		usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
	}

	tidlist = calloc(argc - 1, sizeof(timer_t));
	if (tidlist == NULL) {
		errExit("calloc");
	}

	// establish signal handler for the timer
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(TIMER_SIG, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	// create and start one timer for each command line argument

	// notify via signal
	sev.sigev_notify = SIGEV_SIGNAL;
	// notify using this signal
	sev.sigev_signo = TIMER_SIG;

	for (j = 0; j < argc - 1; j++) {
		// initialize an itimerspec struct from a command line argument (string)
		itimerSpecFromStr(argv[j + 1], &ts);

		// don't need a unique sigevent struct
		// allows handler to get the ID of this timer
		sev.sigev_value.sival_ptr = &tidlist[j];

		// create the timer, store the id in tidlist[j]
		if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1) {
			errExit("timer_create");
		}

		printf("Timer ID: %ld (%s)\n", (long)tidlist[j], argv[j + 1]);

		// arm the timer
		if (timer_settime(tidlist[j], 0, &ts, NULL) == -1) {
			errExit("timer_settime");
		}
	}

	// wait for signals
	for (;;) {
		pause();
	}

	exit(EXIT_SUCCESS);
}
