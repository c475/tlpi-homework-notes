#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


/*
	Simple enough.

	Get flags with fcntl() then xor them, if its 0 they are the same.

	Use lseek() with SEEK_CURR to get file offsets for both fds then compare them.
*/

int same_flags(int fd1, int fd2)
{
	int flags1, flags2;

	flags1 = fcntl(fd1, F_GETFL);
	if (flags1 == -1) {
		errExit("F_GETFL");
	}

	flags2 = fcntl(fd2, F_GETFL);
	if (flags2 == -1) {
		errExit("F_GETFL");
	}

	return !(flags1 ^ flags2);
}


int same_offset(int fd1, int fd2)
{
	int off1, off2;

	off1 = lseek(fd1, 0, SEEK_CURR);
	if (off1 == -1) {
		errExit("lseek");
	}

	off2 = lseek(fd2, 0, SEEK_CURR);
	if (off2 == -1) {
		errExit("lseek");
	}

	return off1 == off2;
}


int main(int argc, char *argv[])
{
	exit(EXIT_SUCCESS);
}
