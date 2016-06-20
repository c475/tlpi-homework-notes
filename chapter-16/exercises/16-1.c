#include <sys/xattr.h>
#include "../../lib/tlpi_hdr.h"


// lol
int main(int argc, char *argv)
{
	if (argc < 4) {
		usageError("%s file name value\n", argv[0]);
	}

	if (setxattr(argv[1], argv[2], argv[3], strlen(argv[3])) == -1) {
		errExit("setxattr");
	}

	exit(EXIT_SUCCESS);
}
