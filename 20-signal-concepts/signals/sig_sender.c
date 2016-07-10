#include <signal.h>
#include "../../lib/tlpi_hdr.h"


// the program that sends signals to the statistics collecting program
int main(int argc, char *argv[])
{
	int numSigs;
	int sig;
	int j;
	pid_t pid;

	if (argc < 4 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
	}

	pid = getLong(argv[1], 0, "PID");
	numSigs = getInt(argv[2], GN_GT_0, "num-sigs");
	sig = getInt(argv[3], 0, "sig-num");

	// send signals to receiver

	printf("%s: sending signal %d to process %ld %d times\n", argv[0], sig, (long)pid, numSigs);

	for (j = 0; j < numSigs; j++) {
		if (kill(pid, sig) == -1) {
			errExit("kill()");
		}
	}

	// if a fourth command-line argument is passed, send that signal too

	if (argc > 4) {
		if (kill(pid, getInt(argv[4], 0, "sig-num-2")) == -1) {
			errExit("kill()");
		}
	}

	printf("%s: exiting\n", argv[0]);

	exit(EXIT_SUCCESS);
}
