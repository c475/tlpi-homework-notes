#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int istack = 222;

	switch (vfork()) {

		case -1:
			errExit("vfork");

		// child executes first, in parents memory space
		case 0:
			// even if you sleep a little, parent is still not scheduled
			sleep(3);

			// can't use stdio functions because they use 
			// statically allocated buffers for user space buffering
			write(STDOUT_FILENO, "Child executing\n", 16);

			// this change will be seen by the parent
			istack *= 3;

			_exit(EXIT_SUCCESS);

		default:
			write(STDOUT_FILENO, "Parent executing\n", 17);
			printf("istack=%d\n", istack);
			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
