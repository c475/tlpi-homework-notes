#define _FILE_OFFSET_BITS 64
#include <fcntl.h>
#include <sys/stat.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int fd;
	off_t off;

	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s pathname offset\n", argv[0]);
	}

	fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		errExit("open");
	}

	off = atoll(argv[2]);
	if (lseek(fd, off, SEEK_SET) == -1) {
		errExit("lseek");
	}

	if (write(fd, "test", 4) == -1) {
		errExit("write");
	}

	exit(EXIT_SUCCESS);
}
