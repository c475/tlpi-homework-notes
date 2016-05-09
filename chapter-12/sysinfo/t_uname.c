#ifdef __linux__
	#define _GNU_SOURCE
#endif

#include <sys/utsname.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	struct utsname uts;

	if (uname(&uts) == -1) {
		errExit("uname");
	}

	printf("Node name:     %s\n", uts.nodename);
	printf("System name:   %s\n", uts.sysname);
	printf("Release:       %s\n", uts.release);
	printf("Version:       %s\n", uts.version);
	printf("Machine:       %s\n", uts.machine);

#ifdef _GNU_SOURCE
	printf("Domain name:   %s\n", uts.domainname);
#endif

	exit(EXIT_SUCCESS);
}
