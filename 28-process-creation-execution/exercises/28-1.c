#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"

/*
	# fork with 2MB memory allocation
	$ time ./28-1 

	real	0m6.824s
	user	0m0.088s
	sys		0m2.756s


	# vfork with 2MB memory allocation
	$ time ./28-1 v

	real	0m1.234s
	user	0m0.020s
	sys		0m0.792s


	Much faster than the author's weak computer.
*/
int main(int argc, char *argv[])
{
	int status;
	pid_t childPid;
	pid_t (*which_fork)(void) = argc > 1 ? vfork : fork;
	char *mem = malloc((1 << 20) * 2); // 2mb
	int i = 0;

	for (i = 0; i < 100000; i++) {
		switch (childPid = which_fork()) {
			case -1:
				errExit("fork/vfork");

			case 0:
				_exit(EXIT_SUCCESS);

			default:
				if (waitpid(childPid, &status, 0) == -1) {
					errExit("waitpid");
				}
		}
	}

	free(mem);

	exit(EXIT_SUCCESS);
}
