#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


/*
	Guessing it's when the grandparent calls wait() and reaps the parent.

	I think that as far as the kernel is concerned the parent is still its own process

	And of course I was wrong, it is adopted when the parent becomes a zombie....
*/

int main(int argc, char *argv[])
{
	int parentStatus;

	printf("Grandparent: %ld\n", (long)getpid());

	switch (fork()) {

		case -1:
			errExit("fork");

		// parent
		case 0:

			printf("Parent: %ld\n", (long)getpid());

			switch (fork()) {

				case -1:
					errExit("fork2");

				// child
				case 0:
					printf("Child: %ld\n", (long)getpid());

					printf("[%s] Before parent exit: %ld\n", currTime("%T"), (long)getppid());
					sleep(2);
					printf("[%s] After parent exit: %ld\n", currTime("%T"), (long)getppid());
					sleep(4);
					printf("[%s] After parent was reaped: %ld\n", currTime("%T"), (long)getppid());
					_exit(EXIT_SUCCESS);

				// still parent
				default:
					sleep(1);
					printf("[%s] Parent exiting\n", currTime("%T"));
					// 2
					_exit(EXIT_SUCCESS);
			}

		// grandparent
		default:
			sleep(5);
			printf("[%s] Grandparent reaping parent\n", currTime("%T"));
			if (wait(&parentStatus) == -1) {
				printf("WAIT FAILED TO REAP PARENT");
			}
			printf("[%s] Parent exit code: %d\n", currTime("%T"), parentStatus);
			sleep(3);
			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
