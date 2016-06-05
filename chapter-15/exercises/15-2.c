#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"


/*
	Q:

	Do you expect any of a file’s three timestamps to be changed by the stat() system
	call? If not, explain why.


	A:

	I expected it to maybe change last access time, but since it does not access the file itself,
	but data in the inode table instead, it leaves all timestamps intact as demonstrated in the program.

	What it DOES do though, is change the last access/modified time for the directory that it resides in.
*/

int main(int argc, char *argv[])
{
	int fd;
	struct stat st;
	struct stat st2;

	if (argc < 2) {
		usageErr("%s filename\n", argv[0]);
	}

	unlink(argv[1]);

	fd = open(argv[1], O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IXUSR);

	close(fd);

	if (stat(argv[1], &st) == -1) {
		errExit("stat() 1");
	}

	printf("last access:       %lld\n", (long long)st.st_atime);
	printf("last modification: %lld\n", (long long)st.st_mtime);
	printf("creation time:     %lld\n\n", (long long)st.st_ctime);

	printf("sleeping for 5s\n\n");

	sleep(5);

	if (stat(argv[1], &st2) == -1) {
		errExit("stat() 2");
	}

	printf("last access:       %lld\n", (long long)st2.st_atime);
	printf("last modification: %lld\n", (long long)st2.st_mtime);
	printf("creation time:     %lld\n\n", (long long)st2.st_ctime);

	exit(EXIT_SUCCESS);
}
