#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int system(const char *command)
{
	int status;
	pid_t childPid;

	switch (childPid = fork()) {

		case -1:
			return -1;

		case 0:
			execl("/bin/sh", "sh", "-c", command, (char *)NULL);

			// failed exec
			_exit(127);

		default:
			if (waitpid(childPid, &status, 0) == -1) {
				return -1;
			} else {
				return status;
			}
	}
}
