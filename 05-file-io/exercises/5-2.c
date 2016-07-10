#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

/*
	spoilers: opening a file with O_APPEND
	will force the offset in the file description to be set to SEEK_END
	on every call to write.

	however, lseek() is still honored... you can use it to read()
	but whenever you write(), write() will automatically seek to the end of the file
*/

int main(int argc, char *argv[])
{
	if (argc != 3) {
		errExit("./program file text");
	}

	int fd = open(argv[1], O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		errExit("open()");
	}

	lseek(fd, 0, SEEK_SET);
	write(fd, argv[2], strlen(argv[2]));
	write(fd, "\n", 1);

	exit(EXIT_SUCCESS);
}

