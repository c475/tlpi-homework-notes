#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int fd = open("lib/", O_RDONLY | O_DIRECTORY);

	if (fd == -1) {
		fprintf(stderr, "OPEN SUCKS\n");
	}

	int num_read;
	char buffer[2048] = {0};

	while((num_read = read(fd, buffer, 2048)) > 0) {

		if (num_read == -1) {
			fprintf(stderr, "read() failed");
		}

		printf("%s", buffer);
		memset(buffer, 0, 2048);
	}

	exit(EXIT_SUCCESS);
}
