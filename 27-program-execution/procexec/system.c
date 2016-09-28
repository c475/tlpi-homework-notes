#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


int system(const char *command)
{
	sigset_t blockMask, origMask;
	struct sigaction saIgnore, saOrigQuit, saOrigInt, saDefault;
	pid_t childPid;
	int status, savedErrno;

	// just check if shell is available
	if (command == NULL) {
		return system(":") == 0;
	}

	// block SIGCHLD
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &blockMask, &origMask);

	// ignore SIGINT and SIGQUIT
	// and save the original handlers for SIGINT and SIGQUIT
	saIgnore.sa_handler = SIG_IGN;
	saIgnore.sa_flags = 0;
	sigemptyset(&saIgnore.sa_mask);
	sigaction(SIGINT, &saIgnore, &saOrigInt);
	sigaction(SIGQUIT, &saIgnore, &saOrigQuit);

	switch (childPid = fork()) {

		// carry on to restore signal attributes
		case -1:
			status = -1;
			break;

		case 0:
			// kinda crazy, but now set the signal handling to that of the "main program"
			saDefault.sa_handler = SIG_DFL;
			saDefault.sa_flags = 0;
			sigemptyset(&saDefault.sa_mask);

			if (saOrigInt.sa_handler != SIG_IGN) {
				sigaction(SIGINT, &saDefault, NULL);
			}

			if (saOrigInt.sa_handler != SIG_IGN) {
				sigaction(SIGQUIT, &saDefault, NULL);
			}

			sigprocmask(SIG_SETMASK, &origMask, NULL);

			execl("/bin/sh", "sh", "-c", command, (char *)NULL);

			// exec failed
			_exit(127);

		default:
			// wait for child to terminate
			while (waitpid(childPid, &status, 0) == -1) {
				// error besides signal interrupt
				if (errno != EINTR) {
					status = -1;
					break;
				}
			}
			break;
	}

	// the following may change errno
	savedErrno = errno;

	// set signal disposition back to normal
	sigprocmask(SIG_SETMASK, &origMask, NULL);
	sigaction(SIGINT, &saOrigInt, NULL);
	sigaction(SIGQUIT, &saOrigQuit, NULL);

	errno = savedErrno;

	return status;
}
