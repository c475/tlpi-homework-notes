#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	int i;
	int res;

	for (i = 0; i < 100000; i++) {
		res = fchdir(i);
		if (res != -1) {
			printf("res: %d\n", res);
		}
	}

	exit(EXIT_SUCCESS);
}
