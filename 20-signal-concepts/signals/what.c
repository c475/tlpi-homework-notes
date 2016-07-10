#include <signal.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	// looks like SIGSTKFLT is used on Linux after all, at least in some capacity (forces restart)
	// kill(-1, SIGSTKFLT);

	// even this forces restart.
	kill(-1, SIGUSR1);
	exit(EXIT_SUCCESS);
}
