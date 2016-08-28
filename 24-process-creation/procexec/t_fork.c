#include "../../lib/tlpi_hdr.h"


static int idata = 111;


int main(int argc, char *argv[])
{
	int istack = 222;
	pid_t childPid;

	switch((childPid = fork())) {

		// error
		case -1:
			errExit("fork");

		// child
		case 0:
			idata *= 3;
			istack *= 3;
			break;

		// parent. should be using wait() here i'd imagine
		default:
			sleep(3);
			break;
	}

	// both child and parent get here
	printf(
		"PID=%ld %s idata=%d istack=%d\n",
		(long)getpid(),
		childPid == 0 ? "(child) " : "(parent) ",
		idata,
		istack
	);

	exit(EXIT_SUCCESS);
}
