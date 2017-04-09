#include <signal.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"

#define SYNCSIGNAL SIGUSR1


void handler(int sig)
{

}


void consume(int length)
{
	struct tms timebuffer;
	clock_t ticksPerSecond;
	double cpuTime;

	ticksPerSecond = sysconf(_SC_CLK_TCK);

	for (;;) {
		times(&timebuffer);

		cpuTime = (double)timebuffer.tms_utime / (double)ticksPerSecond;

		if (cpuTime >= length) {
			break;
		}
	}
}


int main(int argc, char *argv[])
{
	int status;
	struct sigaction sa;
	struct rusage usage;
	pid_t childPid;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction(SYNCSIGNAL, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	childPid = fork();
	if (childPid == -1) {
		errExit("fork");
	}

	if (childPid == 0) {
		childPid = getpid();
		// consume 2 seconds of CPU time
		printf("[%ld] Consuming 2 seconds of user CPU time\n", (long)childPid);
		consume(2);
		// wake parent up
		printf("[%ld] Waking parent process up and pausing\n\n", (long)childPid);
		if (kill(getppid(), SYNCSIGNAL) == -1) {
			errExit("kill");
		}
		pause();
		printf("[%ld] Child exiting\n", (long)childPid);
		_exit(EXIT_SUCCESS);
	} else {
		printf("[%ld] Parent sleeping while child consumes CPU\n", (long)getpid());
		pause();
		if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
			errExit("getrusage");
		}

		// print usage info on child process before it exits
		printf("========== BEFORE WAIT ==========\n");
		printf("USER S: %lld, USER MS: %lld\n", 
			(long long)usage.ru_utime.tv_sec, 
			(long long)usage.ru_utime.tv_usec
		);
		printf("SYST S: %lld, SYST MS: %lld\n\n", 
			(long long)usage.ru_stime.tv_sec, 
			(long long)usage.ru_stime.tv_usec
		);

		// wake child up to exit
		if (kill(childPid, SYNCSIGNAL) == -1) {
			errExit("kill");
		}

		// then wait on dead child process
		if (wait(&status) == -1) {
			errExit("wait");
		}

		printf("[%ld] Parent reaped child process\n\n", (long)getpid());

		if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
			errExit("getrusage");
		}

		// print usage info on child process after it exits
		printf("========== AFTER WAIT ==========\n");
		printf("USER SEC: %lld, USER MS: %lld\n",
			(long long)usage.ru_utime.tv_sec,
			(long long)usage.ru_utime.tv_usec
		);
		printf("SYST SEC: %lld, SYST MS: %lld\n\n",
			(long long)usage.ru_stime.tv_sec,
			(long long)usage.ru_stime.tv_usec
		);
	}

	exit(EXIT_SUCCESS);
}
