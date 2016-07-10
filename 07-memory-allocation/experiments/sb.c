#define _BSD_SOURCE
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	char *x = sbrk(10000);
	exit(EXIT_SUCCESS);
}
