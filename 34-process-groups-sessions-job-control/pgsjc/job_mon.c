#define _GNU_SOURCE // strsignal()
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

static int cmdNum; // our position in pipeline


static void handler(int sig)
{
	// if process group leader
	if (getpid() == getpgrp()) {
		fprintf(stderr, "Terminal FG process group: %ld\n", (long)tcgetpgrp(STDERR_FILENO));
	}

	fprintf(stderr, "Process %ld (%d) received signal %d (%s)\n",
		(long)getpid(), cmdNum, sig, strsignal(sig));

	// if we catch SIGTSTP it wont actually stop the process
	// so you have to raise SIGSTOP
	if (sig == SIGTSTP) {
		raise(SIGSTOP);
	}
}


int main(int argc, char *argv[])
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		errExit("sigaction");
	}
	if (sigaction(SIGTSTP, &sa, NULL) == -1) {
		errExit("sigaction");
	}
	if (sigaction(SIGCONT, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	// if stdin is a terminal, this is the first process in the pipeline:
	// print a heading and initialize message to be sent down pipe
	if (isatty(STDIN_FILENO)) {
		fprintf(stderr, "Terminal FG process group: %ld\n",
			(long)tcgetpgrp(STDIN_FILENO));
		fprintf(stderr, "Command   PID  PPID  PGRP    SID\n");
		cmdNum = 0;

	// not first in the pipeline, so read message from pipe
	} else {
		if (read(STDIN_FILENO, &cmdNum, sizeof(cmdNum)) <= 0) {
			fatal("read git EOF or error");
		}
	}

	cmdNum++;

	fprintf(stderr, "%4d    %5ld %5ld %5ld %5ld\n",
		cmdNum, (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(0));

	// if not the last process, pass a message to the next process
	// if not tty, then should be a pipe
	if (!isatty(STDOUT_FILENO)) {
		if (write(STDOUT_FILENO, &cmdNum, sizeof(cmdNum)) == -1) {
			errMsg("write");
		}
	}

	for (;;) {
		pause();
	}

	exit(EXIT_FAILURE);
}
