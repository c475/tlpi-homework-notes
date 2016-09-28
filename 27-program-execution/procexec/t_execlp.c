#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s pathname\n", argv[0]);
	}

	execlp(argv[1], argv[1], "hello world", (char *)NULL);
	errExit("execlp");
}
