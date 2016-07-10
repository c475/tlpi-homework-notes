#include <stdio.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	char buf[1024] = {0};

	if (read(STDIN_FILENO, buf, 1024) == -1) {
		errExit("read()");
	}

	int i = 0;
	for(i = 0; i < 1024; i++) {
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
	}

	printf("%lld\n", *((long long *)buf));

	char *ok = malloc(*((long long *)buf));

	if (ok == NULL) {
		ok = ok[4342];
	}

	long int num = getLong(buf, GN_ANY_BASE, "testname");

	if (write(STDOUT_FILENO, buf, 1024) == -1) {
		errExit("write()");
	}


	printf("%ld\n", num);

	memset(buf, 0, 1024);

	free(ok);

	exit(EXIT_SUCCESS);
}
