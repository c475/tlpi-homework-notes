#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"

#define MAX_CMD_LEN 200


int main(int argc, char *argv[])
{
	char str[MAX_CMD_LEN];
	int status;

	for (;;) {
		printf("Command: ");
		fflush(stdout);

		// get a command
		if (fgets(str, MAX_CMD_LEN, stdin) == NULL) {
			// EOF
			break;
		}

		status = system(str);

		printf("system() returned: status=0x%04x (%d,%d)\n",
			(unsigned int)status,
			status >> 8,
			status & 0xFF
		);

		if (status == -1) {
			errExit("system");
		} else {
			if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
				printf("(Probaby) could not invoke shell\n");
			} else {
				printWaitStatus(NULL, status);
			}
		}
	}

	exit(EXIT_SUCCESS);
}
