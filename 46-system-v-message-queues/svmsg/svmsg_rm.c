#include <sys/types.h>
#include <sys/msg.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int i;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		usageErr("%s [msqid...]\n", argv[0]);
	}

	for (i = 1; i < argc; i++) {
		if (msgctl(getInt(argv[i], 0, "msqid"), IPC_RMID, NULL) == -1) {
			errExit("msgctl");
		}
	}

	exit(EXIT_SUCCESS);
}
