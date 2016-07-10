#include <sys/stat.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"

#define BUF_SIZE PATH_MAX


int main(int argc, char *argv[])
{
	struct stat statbuf;
	char buf[BUF_SIZE];
	ssize_t numBytes;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s pathname\n", argv[0]);
	}

	if (lstat(argv[1], &statbuf) == -1) {
		errExit("lstat");
	}

	if (!S_ISLNK(statbuf.st_mode)) {
		fatal("%s is not a sumbolic link", argv[1]);
	}

	numBytes = readlink(argv[1], buf, BUF_SIZE - 1);
	if (numBytes == -1) {
		errExit("readlink");
	}

	buf[numBytes] = '\0';

	printf("readlink: %s --> %s\n", argv[1], buf);

	if (realpath(argv[1], buf) == NULL) {
		errExit("realpath");
	}

	printf("realpath: %s --> %s\n", argv[1], buf);

	exit(EXIT_SUCCESS);
}
