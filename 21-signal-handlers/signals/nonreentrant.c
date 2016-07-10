#define _BSD_SOURCE // get getpass() declaration from <unistd.h>
#define _XPOEN_SOURCE 600 // get crypt() declaration from <unistd.h>

#include <crypt.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "../../lib/tlpi_hdr.h"


static char *str2;			// set from argv[2]
static int handled = 0; 	// counts number of calls to handler


static void handler(int sig)
{
	crypt(str2, "xx");
	handled++;
}


int main(int argc, char *argv[])
{
	char *cr1;
	int callNum;
	int mismatch;
	struct sigaction sa;

	if (argc != 3) {
		usageErr("%s str1 str2\n", argv[0]);
	}

	// make argv[2] available to the handler
	str2 = argv[2];

	// make a heap copy of a statically allocated buffer (returned by crypt)
	cr1 = strdup(crypt(argv[1], "xx"));
	if (cr1 == NULL) {
		errExit("strdup");
	}

	// you can just do this it turns out...
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		errExit("sigaction()");
	}

	/*
		Repeatedly call crypt() using argv[1]. If interrupted by a
		signal handler, then the static storage returned by crypt()
		will be overwritten by the results of encrypting argv[2], and
		strcmp() will detect a mismatch with the value in 'cr1'.

		IN OTHER WORDS, crypt() USES THE SAME STATIC STORAGE TO ENCRYPT ALL STRINGS OVER THE PROGRAM'S LIFETIME.
	*/

	for (callNum = 1, mismatch = 0; ; callNum++) {
		if (strcmp(crypt(argv[1], "xx"), cr1) != 0) {
			mismatch++;
			printf("Mismatch on call %d (mismatch=%d, handled=%d)\n", callNum, mismatch, handled);
		}
	}

	exit(EXIT_SUCCESS);
}
