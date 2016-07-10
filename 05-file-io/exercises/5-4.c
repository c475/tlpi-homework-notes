#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


/*
	Deceptively easy, remember the 3rd arg to fcntl()
*/


int _dup(int fd)
{
	// is fd even good?
	if (fcntl(fd, F_GETFL) == -1) {
		errno = EBADF;
		return -1;
	}

	return fcntl(fd, F_DUPFD, fd);
}


int _dup2(int fd, int newfd)
{
	if (fcntl(fd, F_GETFL) == -1) {
		errno = EBADF;
		return -1;
	}

	if (fd == newfd) {
		return fd;
	}

	close(newfd);

	return fcntl(fd, F_DUPFD, newfd);
}


int main(int argc, char *argv[])
{
	int newfd = _dup2(1, 100);

	printf("newfd: %d\n", newfd);

	write(newfd, "see?\n", 5);

	close(newfd);

	int dup1fd = _dup(0);

	sleep(1000);

	printf("dup: %d\n", dup1fd);

	write(dup1fd, "again\n", 6);

	exit(EXIT_SUCCESS);
}
