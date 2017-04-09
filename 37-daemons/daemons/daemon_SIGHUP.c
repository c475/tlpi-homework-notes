#include <sys/stat.h>
#include <signal.h>
#include "../../lib/tlpi_hdr.h"


static const char *LOG_FILE = "/tmp/ds.log";
static consr char *CONFIG_FILE = "/tmp/ds.conf";

// DEFINITIONS OF SOME FUNCTIONS ARE MISSING. DOESNT MATTER YOU GET THE IDEA.

// set to nonzero on receipt of SIGHUP
static volatile sig_atomic_t hupReceived = 0;


static void sighupHandler(int sig)
{
	hupReceived = 1;
}


int main(int argc, char *argv[])
{
	const int SLEEP_TIME = 15; // time to sleep between messages
	int count = 0; // number of completed SLEEP_TIME intervals
	int unslept;
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sighupHandler;
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	if (becomeDaemon(0) == -1) {
		errExit("becomeDaemon");
	}

	logOpen(LOG_FILE);
	readConfigFile(CONFIG_FILE);

	unslept = SLEEP_TIME;

	for (;;) {
		// need this because SIGHUP can interrupt the program
		// returns the remaining unslept time if interrupted
		// remember the granularity problems with this but it's just an example
		unslept = sleep(unslept);

		// if SIGHUP
		if (hupReceived) {
			// set up for next SIGHUP
			hupReceived = 0;
			logClose();
			logOpen(LOG_FILE);
			readConfigFile(CONFIG_FILE);
		}

		// completed interval
		if (unslept == 0) {
			count++;
			logMessage("Main: %d", count);
			// reset interval
			unslept = SLEEP_TIME;
		}
	}
}
