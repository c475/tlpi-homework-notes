#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	if (fork() == 0) {
		sleep(1);
		printf("PARENT PID: %ld\n", (long)getppid());
	}

	exit(EXIT_SUCCESS);
}
