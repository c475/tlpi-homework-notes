#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


/*
	O_APPEND will update the offset in the file description every write()
	so it is safe to share between threads/processes.

	lseek() will fuck it all up
*/


int main(int argc, char *argv[])
{
	if (argc < 2) {
		errExit("./program filename num-bytes [x]");
	}

	int flags = O_WRONLY | O_CREAT | (argc >= 4 ? 0 : O_APPEND);
	int fd = open(argv[1], flags, S_IRUSR | S_IWUSR);
	int i = atol(argv[2]);

	while (i--) {

		if (argc >= 4) {
			lseek(fd, 0, SEEK_END);
		}

		write(fd, "a", 1);
	}

	close(fd);

	exit(EXIT_SUCCESS);
}
