#include <unistd.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	vfork();
	vfork();
	vfork();

	printf("PID: %ld\n", (long)getpid());

	_exit(EXIT_SUCCESS);
}
