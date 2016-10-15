#define _GNU_SOURCE
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h> // clone
#include "../../lib/tlpi_hdr.h"


#ifndef CHILD_SIG

#define CHILD_SIG SIGUSR1 // on termination of cloned child

#endif


// thread function
static int childFunc(void *arg)
{
	if (close(*((int *)arg)) == -1) {
		errExit("close");
	}

	return 0;
}


int main(int argc, char *argv[])
{
	const int STACK_SIZE = 65536;
	char *stack;
	char *stackTop;
	int s;
	int fd;
	int flags;

	fd = open("/dev/null", O_RDWR);
	if (fd == -1) {
		errExit("open");
	}

	// if argc > 1, child shares descriptor table with parent
	// INSTEAD OF BEING DUPICATED, that is why close() succeeds in childFunc()
	flags = argc > 1 ? CLONE_FILES : 0;

	// allocate stack for the child
	stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		errExit("malloc");
	}

	// have to pass the top of the stack, assuming it grows downwards
	stackTop = stack + STACK_SIZE;

	/*
		Ignore CHILD_SIG, in case it is a signal whose default is to
		terminate the process; but don't ignore SIGCHLD (which is ignored
		by default), since that would prevent the creation of a zombie.
	*/
	if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD) {
		if (signal(CHILD_SIG, SIG_IGN) == SIG_ERR) {
			errExit("signal");
		}
	}

	// create child, child executes childFunc()
	if (clone(childFunc, stackTop, flags | CHILD_SIG, (void *)&fd) == -1) {
		errExit("clone");
	}

	// parent falls thrOugh here, wait for child
	// __WCLONE is needed for child notifying parent with signal (exiting)
	// other than SIGCHLD (which only clone() can do)
	if (waitpid(-1, NULL, CHILD_SIG != SIGCHLD ? __WCLONE : 0) == -1) {
		errExit("waitpid");
	}

	printf("child was terminated\n");

	s = write(fd, "x", 1);
	if (s == -1 && errno == EBADF) {
		printf("File descriptor %d has been closed\n", fd);
	} else if (s == -1) {
		printf("write() on file descriptor %d failed unexpectedlt (%s)\n", fd, strerror(errno));
	} else {
		printf("write() on file descriptor %d succeeded\n", fd);
	}

	exit(EXIT_SUCCESS);
}
