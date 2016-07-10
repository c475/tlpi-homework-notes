#include <signal.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int s;
	int sig;

	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s pid sig-num\n", argv[0]);
	}

	sig = getInt(argv[2], 0, "sig-num");

	s = kill(getLong(argv[1], 0, "pid"), sig);

	if (sig != 0) {
		if (sig == -1) {
			errExit("kill()");
		}
	} else {
		if (s == 0) {
			printf("process exists and the program can send it a signal\n");
		} else {
			if (errno == EPERM) {
				printf("Process exists, but don't have permission to send it a signal");
			} else if (errno == ESRCH) {
				printf("Process does not exist\n");
			} else {
				errExit("kill()");
			}
		}
	}

	exit(EXIT_SUCCESS);
}
