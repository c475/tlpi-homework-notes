#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define CMD_SIZE 200
#define BUF_SIZE 1024


int main(int argc, char *argv[])
{
	int fd;
	int j;
	int numBlocks;
	char shellCmd[CMD_SIZE];
	char buf[BUF_SIZE];

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s temp-file [num-1kb-blocks] \n", argv[0]);
	}

	numBlocks = argc > 2 ? getInt(argv[2], GN_GT_0, "num-1kb-blocks") : 100000;

	// create a file that will be immediately unlinked
	fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		errExit("open()");
	}

	// unlink the file that was just created
	if (unlink(argv[1]) == -1) {
		errExit("unlink()");
	}

	// write junk to the file (that is unlinked)
	for (j = 0; j < numBlocks; j++) {
		if (write(fd, buf, BUF_SIZE) != BUF_SIZE) {
			fatal("partial/failed write");
		}
	}

	snprintf(shellCmd, CMD_SIZE, "df -k `dirname %s`", argv[1]);
	system(shellCmd);

	if (close(fd) == -1) {
		errExit("close()");
	}

	printf("********** Closed file descriptor\n");

	system(shellCmd);

	exit(EXIT_SUCCESS);
}
