#include <signal.h>
#include "../../lib/tlpi_hdr.h"

/*
	The signal handler terminates the sleep() in progress, interesting

	$ ./ouch 
	0
	^COuch!
	1
	^COuch!
	2
	^COuch!
	3
	^COuch!
	4
	^COuch!
	5
	^COuch!
	6
	^COuch!
	7
	^COuch!
	8
	^COuch!
	9
	^COuch!
*/



static void sigHandler(int sig)
{
	// UNSAFE (section 21.1.2)
	printf("Ouch!\n");
}


int main(int argc, char *argv[])
{
	int j;

	if (signal(SIGINT, sigHandler) == SIG_ERR) {
		errExit("signal()");
	}

	for (j = 0; j < 10; j++) {
		printf("%d\n", j);
		// loop slowly
		// sleep gets interrupted by sigint, immediately jumps to top of loop and prints number
		sleep(3);
	}

	exit(EXIT_SUCCESS);
}
