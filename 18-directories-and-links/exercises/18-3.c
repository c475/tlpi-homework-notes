#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>
#include "../../lib/tlpi_hdr.h"

#define BUF_SIZE PATH_MAX


char *t_realpath(const char *pathname)
{
	int fd1;	// current working directory
	int fd2;	// destination directory

	char *cwd;
	char *path_copy;
	char *pseg;

	if (access(pathname, F_OK) == -1) {
		return NULL;
	}

	path_copy = strdup(pathname);
	pseg = dirname(path_copy);

	fd1 = open(".", O_RDONLY);
	fd2 = open(pseg, O_RDONLY);

	if (fd1 == -1 || fd2 == -1 || fchdir(fd2) == -1) {
		if (fd1 != -1) close(fd1);
		if (fd2 != -1) close(fd2);
		return NULL;
	}

	cwd = calloc(1, PATH_MAX);
	if (cwd == NULL) {
		return NULL;
	}

	if (getcwd(cwd, PATH_MAX) == NULL) {
		close(fd1);
		close(fd2);
		free(cwd);
		return NULL;
	}

	free(path_copy);
	path_copy = strdup(pathname);
	pseg = basename(path_copy);

	strncat(cwd, "/", 1);
	strncat(cwd, pseg, PATH_MAX - strlen(cwd));

	free(path_copy);

	if (fchdir(fd1) == -1) {
		close(fd1);
		close(fd2);
		free(cwd);
		return NULL;
	}

	close(fd1);
	close(fd2);

	return cwd;
}


int main(int argc, char *argv[])
{
	if (argc != 2) {
		usageErr("%s file\n", argv[0]);
	}

	char *real = t_realpath(argv[1]);

	if (real) {
		printf("%s\n", real);
		free(real);
	} else {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
