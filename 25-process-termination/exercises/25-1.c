#include <sys/wait.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


/*
	"If a child process makes the call exit(–1), what exit status will be seen by the parent?"

	My guess is that it will return 255 because it will only use the first 8 bits of the code?

	That or 0.


	Well, I was part right.

	It returns 65280, which is:

	1111111100000000

	So it uses the first 8 bts of the second octet. Weird.

	Testing signal death of the child:
	
	raise(SIGINT);

	Returns: 2

	0000000000000010

	So on this machine, it uses the second octet for the return code, and the first for signal number.

	This is the opposite of the machine in the book.
*/
int main(int arhc, char *argv[])
{
	int status;

	switch (fork()) {

		case -1:
			errExit("fork()");

		case 0:
			raise(SIGINT);

		default:
			wait(&status);
			printf("status is: %d\n", status);
			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
