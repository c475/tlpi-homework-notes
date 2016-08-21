#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "../../lib/tlpi_hdr.h"


void handler(int sig)
{
	printf("[%s] signal caught...\n", currTime("%T"));
}


unsigned int _alarm(unsigned int seconds)
{
	struct itimerval new, old;

	memset(&new, 0, sizeof(struct itimerval));

	new.it_value.tv_sec = seconds;

	if (setitimer(ITIMER_REAL, &new, &old) == -1) {
		return -1;
	}

	return old.it_value.tv_sec;
}


// can easily see the low resolution.
int main(int argc, char *argv[])
{
	int t, first, second, sleeptime;
	struct sigaction sa;

	// argv[1]: seconds for first call to _alarm()
	// argv[2]: seconds for second call to _alarm()
	// argv[3]: seconds to sleep between the two calls to _alarm()
	if (argc != 4) {
		usageErr("%s first-alarm second-alarm sleep\n", argv[0]);
	}

	first = atoi(argv[1]);
	second = atoi(argv[2]);
	sleeptime = atoi(argv[3]);

	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	t = _alarm(first);

	printf("[%s] Sleeping for %d seconds. _alarm() returned %d\n",
		currTime("%T"),
		first,
		t
	);

	sleep(sleeptime);

	t = _alarm(second);

	printf("[%s] Sleeping for %d seconds. Second call to _alarm() returned %d\n",
		currTime("%T"),
		second,
		t
	);

	pause();

	exit(EXIT_SUCCESS);
}
