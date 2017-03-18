#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	if (setsid() == -1) {
		printf("yep it failed\n");
	}

	exit(EXIT_SUCCESS);
}
