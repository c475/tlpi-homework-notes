#include <fcntl.h>
#include <unistd.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int i;
	char sysbuf[4098] = {0};
	char sysbuf2[4098] = {0};

	if (argc != 2) {
		exit(EXIT_FAILURE);
	}

	snprintf(sysbuf, 4098, "touch %s/%s", argv[1], "testfile");
	snprintf(sysbuf2, 4098, "rm -f %s/%s", argv[1], "testfile");

	for (i = 0; i < 1000; i++) {
		system(sysbuf);
		system(sysbuf2);
	}

	exit(EXIT_SUCCESS);
}
