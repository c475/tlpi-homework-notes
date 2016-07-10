#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define BUF_SIZE 1024


int main(int argc, char *argv[])
{
	int fd;
	char buf[BUF_SIZE];

	// signed size_t to catch -1 errors
	ssize_t n;

	fd = open("/proc/sys/kernel/pid_max", (argc > 1) ? O_RDWR : O_RDONLY);
	if (fd == -1) {
		errExit("open");
	}

	n = read(fd, buf, BUF_SIZE);
	if (n == -1) {
		errExit("read");
	}

	if (argc > 1) {
		printf("Old value: ");
	}

	printf("%.*s", (int)n, buf);

	if (argc > 1) {
		size_t len = strlen(argv[1]);
		int written = write(fd, argv[1], len);

		printf("len: %ld, writren: %ld\n", (long)len, (long)written);

		if (written != len) {
			errExit("write() failed");
		}

		system("echo /proc/sys/kernel/pid_max now contains `cat /proc/sys/kernel/pid_max`");
	}

	exit(EXIT_SUCCESS);
}
