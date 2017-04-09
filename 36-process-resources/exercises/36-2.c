#include <sys/wait.h>
#include <sys/resource.h>
#include "../../lib/tlpi_hdr.h"


static pid_t childPid = 0;


void handler(int sig)
{
	if (kill(childPid, SIGKILL) == -1) {
		errExit("kill");
	}
}


int main(int argc, char *argv[])
{
	int status;
	struct rusage usage;
	struct sigaction sa;
	struct sigaction old;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	if (sigaction(SIGINT, &sa, &old) == -1) {
		errExit("sigaction");
	}

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s cmd [arg]...\n", argv[0]);
	}

	switch (childPid = fork()) {

		case -1:
			errExit("fork");

		case 0:
			if (sigaction(SIGINT, &old, NULL) == -1) {
				errExit("sigaction");
			}
			execvp(argv[1], argv + 1);
			_exit(EXIT_FAILURE);

		default:
			if (wait(&status) == -1 && errno != EINTR) {
				errExit("wait");
			}

			if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
				errExit("getrusage");
			}

			printf("\nuser:   %lld.%lld\n", 
				(long long)usage.ru_utime.tv_sec, 
				(long long)usage.ru_utime.tv_usec
			);
			printf("sys:    %lld.%lld\n", 
				(long long)usage.ru_stime.tv_sec, 
				(long long)usage.ru_stime.tv_usec
			);
	}


	exit(EXIT_SUCCESS);
}
