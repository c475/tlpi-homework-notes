#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	char *argVec[10];
	char *envVec[] = { "GREET=salut", "BYE=adieu", NULL };

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s pathname\n", argv[0]);
	}

	argVec[0] = strrchr(argv[1], '/'); // get basename
	if (argVec[0] != NULL) {
		argVec[0]++;
	} else {
		argVec[0] = argv[1];
	}

	argVec[1] = "hello world";
	argVec[2] = "goodbye";
	argVec[3] = NULL;

	execve(argv[1], argVec, envVec);

	errExit("execve");
}
