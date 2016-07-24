#include <signal.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


void handler(int sig, siginto_t *info, void *ucontext)
{
	if (sig == SIGINT) {
		exit(EXIT_SUCCESS);
	}

	printf("Signal: %s(%d)\nCode: %d\nValue: %d\nPID: %ld\nUtime: %ld\nSystime: %ld\n",
		strsignal(info->si_signo),
		info->si_signo,
		info->si_code,
		info->si_value,
		(long)info->si_pid,
		(long)info->si_utime,
		(long)info->si_stime
	);
}


int main(int argc, char *argv[])
{
	int i;
	struct sigaction sa;

	sa.handler = handler;
	sigfillset(&sa.sa_mask);
	sa.sa_mask = SA_SIGINFO;

	for (i = 1; i < NSIG; i++) {
		if (sigaction(i, &sa, NULL) == -1) {
			errExit("sigaction()");
		}
	}

	for (;;) {
		sleep(0.3);
	}

	exit(EXIT_SUCCESS);
}
