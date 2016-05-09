#include "../../lib/tlpi_hdr.h"


static void sysconfPrint(const char *msg, int name)
{
	long lim;
	errno = 0;

	lim = sysconf(name);

	// success
	if (lim != -1) {
		printf("%s %ld\n", msg, lim);

	// something went wrong
	} else {
		// no error but could not determine limit
		if (errno == 0) {
			printf("%s indeterminate\n", msg);

		// actual error
		} else {
			errExit("sysconf %s", msg);
		}
	}
}


int main(int argc, char *argv[])
{
	sysconfPrint("_SC_ARG_MAX:\t\t", _SC_ARG_MAX);
	sysconfPrint("_SC_LOGIN_NAME_MAX:\t\t", _SC_LOGIN_NAME_MAX);
	sysconfPrint("_SC_OPEN_MAX:\t\t", _SC_OPEN_MAX);
	sysconfPrint("_SC_NGROUPS_MAX:\t\t", _SC_NGROUPS_MAX);
	sysconfPrint("_SC_PAGESIZE:\t\t", _SC_PAGESIZE);
	sysconfPrint("_SC_RTSIG_MAX:\t\t", _SC_RTSIG_MAX);

	exit(EXIT_SUCCESS);
}
