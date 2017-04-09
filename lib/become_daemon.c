#include <sys/stat.h>
#include <fcntl.h>
#include "become_daemon.h"
#include "tlpi_hdr.h"


// returns 0 on success, or -1 on error
int becomeDaemon(int flags)
{
	int maxfd;
	int fd;

	switch (fork()) {

		case -1:
			return -1;

		// child falls through
		case 0:
			break;

		// parent terminates, child orphaned
		default:
			_exit(EXIT_SUCCESS);
	}

	// become leader of new session
	if (setsid() == -1) {
		return -1;
	}

	// ensure that it is not session leader
	// also ensures the daemon cannot reopen a controlling terminal
	switch (fork()) {

		case -1:
			return -1;

		// child falls through
		case 0:
			break;

		// parent terminates, child orphaned
		default:
			_exit(EXIT_SUCCESS);
	}

	if (!(flags & BD_NO_UMASK0)) {
		// clear file creation mask
		umask(0);
	}

	if (!(flags & BD_NO_CHDIR)) {
		// change to root directory
		chdir("/");
	}

	if (!(flags & BD_NO_CLOSE_FILES)) {
		maxfd = sysconf(_SC_OPEN_MAX);
		// limit is indeterminate
		if (maxfd == -1) {
			// take a guess
			maxfd = BD_MAX_CLOSE;
		}

		for (fd = 0; fd < maxfd; fd++) {
			close(fd);
		}
	}

	if (!(flags & BD_NO_REOPEN_STD_FDS)) {
		// reopen standard fd's to /dev/null
		close(STDIN_FILENO);

		fd = open("/dev/null", O_RDWR);

		// fd should be 0
		if (fd != STDIN_FILENO) {
			return -1;
		}

		if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
			return -1;
		}

		if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
			return -1;
		}
	}

	return 0;
}
