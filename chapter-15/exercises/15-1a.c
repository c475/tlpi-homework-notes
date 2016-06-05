#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int fd;

	if (argc < 2) {
		usageErr("%s filename\n", argv[0]);
	}

	unlink(argv[1]);

	fd = open(argv[1], O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd == -1) {
		errExit("file already exists");
	}

	// remove all owner permissions
	if (fchmod(fd, S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP) == -1) {
		errExit("fchmod()");
	}

	close(fd);

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		errExit("a) unable to open for reading, good");
	}

	exit(EXIT_FAILURE);
}
