#include <signal.h>
#include "../../lib/tlpi_hdr.h"


void print_signal(siginfo_t *info)
{
	fprintf(stderr, "Signal: %s(%d)\nCode: %d\nValue: %d\nPID: %ld\nUtime: %ld\nSystime: %ld\n",
		strsignal(info->si_signo),
		info->si_signo,
		info->si_code,
		info->si_value.sival_int,
		(long)info->si_pid,
		(long)info->si_utime,
		(long)info->si_stime
	);
}


void handler(int sig, siginfo_t *info, void *ucontext)
{
	print_signal(info);
}


// delivers standard signals first...
int main(int argc, char *argv[])
{
	int i;
	sigset_t newMask;
	sigset_t oldMask;
	struct sigaction sa;

	printf("PID: %ld\n", (long)getpid());

	printf("sigrtmin: %d\n", (int)SIGRTMIN);

	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);

	sigfillset(&newMask);

	printf("blocking all signals...\n");

	if (sigprocmask(SIG_SETMASK, &newMask, &oldMask) == -1) {
		errExit("sigprocmask");
	}

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		errExit("sigaction1");
	}

	for (i = SIGRTMIN; i <= SIGRTMAX; i++) {
		if (sigaction(i, &sa, NULL) == -1) {
			errExit("sigaction");
		}
	}

	printf("raising SIGINT, then sleeping 20 seconds, send a realtime signal to me\n");

	raise(SIGINT);

	sleep(20);

	printf("unblocking all signals\n");

	if (sigprocmask(SIG_SETMASK, &oldMask, NULL) == -1) {
		errExit("sigprocmask2");
	}

	exit(EXIT_SUCCESS);
}
