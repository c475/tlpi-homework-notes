#include <fcntl.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"

#define CONTENT_MAX 1024*4


void cleanup(char *content, char *pidpath, char *fdpath)
{
	if (content) {
		memset(content, 0, CONTENT_MAX);
	}

	if (pidpath) {
		memset(pidpath, 0, PATH_MAX);
	}

	if (fdpath) {
		memset(fdpath, 0, PATH_MAX);
	}
}


int main(int argc, char *argv[])
{
	DIR *proc_dp;
	DIR *fd_dp;

	Status *status;

	struct dirent *pid_dir;
	struct dirent *fd_dir;

	char pathname[PATH_MAX] = {0};
	char fd_pathname[PATH_MAX] = {0};
	char content[CONTENT_MAX] = {0};

	if (argc != 2) {
		usageErr("./program filename\n");
	}

	proc_dp = opendir("/proc/");

	while ((pid_dir = readdir(proc_dp)) != NULL) {

		if (isNumericString(pid_dir->d_name)) {

			snprintf(pathname, PATH_MAX, "/proc/%s/fd/", pid_dir->d_name);

			fd_dp = opendir(pathname);

			if (!fd_dp) {
				cleanup(NULL, pathname, NULL);
				continue;
			}

			while ((fd_dir = readdir(fd_dp)) != NULL) {

				snprintf(fd_pathname, PATH_MAX, "/proc/%s/fd/%s",
					pid_dir->d_name,
					fd_dir->d_name
				);

				if (readlink(fd_pathname, content, CONTENT_MAX - 1) == -1) {
					cleanup(NULL, NULL, fd_pathname);
					continue;
				}

				status = getProcessStatus(atoi(pid_dir->d_name));
				if (status == NULL) {
					continue;
				}

				if (endswith(content, argv[1])) {
					printf("%s (%ld)\n", status->comm, (long)status->pid);
				}

				free(status);
				cleanup(content, NULL, fd_pathname);
			}

			cleanup(NULL, pathname, NULL);
		}
	}

	closedir(proc_dp);
	closedir(fd_dp);

	exit(EXIT_SUCCESS);
}
