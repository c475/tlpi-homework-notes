#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	int i;
	int fd;
	int newfd;

	fd = open("test.txt", O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IXUSR);

	for (i = 0; i < 1000; i++) {
		newfd = dup2(fd, i);
		printf("%d\n", i);
	}

	exit(EXIT_SUCCESS);
}
