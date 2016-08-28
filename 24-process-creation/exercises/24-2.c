#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int fd;
	int status;
	char buf[1024];

	memset(buf, 0, 1024);

	fd = open("testfile.txt", O_CREAT | O_TRUNC | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd == -1) {
		errExit("open");
	}

	switch (vfork()) {

		case -1:
			errExit("vfork");

		case 0:
			write(fd, "test1\n", 6);
			printf("Closing file decriptor in child\n");
			close(fd);
			_exit(EXIT_SUCCESS);

		default:
			wait(&status);
			lseek(fd, 0, SEEK_SET);
			printf("Reading from the same file descriptor in parent:\n");
			if (read(fd, buf, 1024) == -1) {
				printf("read(): Bad file descriptor\n");
				exit(EXIT_FAILURE);
			}
			printf("%s", buf);
			exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);
}
