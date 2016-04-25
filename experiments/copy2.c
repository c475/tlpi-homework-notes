#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../lib/tlpi_hdr.h"

#define BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE] = {0};
	ssize_t num_read;
	ssize_t num_written;
	int src;
	int dst;

	if (argc != 3) {
		errExit("./copy2 src dst");
	}

	src = open(argv[1], O_RDONLY);

	if (src == -1) {
		errExit("src");
	}

	dst = open(
		argv[2],
		O_CREAT | O_TRUNC | O_WRONLY,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
	);

	if (dst == -1) {
		errExit("dst");
	}

	while ((num_read = read(src, buffer, BUFFER_SIZE)) > 0) {

		if (num_read == -1) {
			errExit("read()");
		}

		num_written = write(dst, buffer, num_read);

		if (num_written == -1 || num_written != num_read) {
			errExit("write() to src");
		}

		memset(buffer, 0, num_read);
	}

	if (close(src) == -1) {
		errExit("close() src");
	}

	if (close(dst) == -1) {
		errExit("close() dst");
	}

	exit(EXIT_SUCCESS);
}
