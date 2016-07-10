#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"


char *t_getcwd(void)
{
	int cwd_save;
	size_t len_c;
	size_t len_d;
	struct dirent *direntp;
	struct stat statbuf;
	ino_t dir_inode;
	char *current;
	char *tmp;
	DIR *dirp;

	current = NULL;

	cwd_save = open(".", O_RDONLY);
	if (cwd_save == -1) {
		return NULL;
	}

	if (stat(".", &statbuf) == -1) {
		return NULL;
	}

	dir_inode = statbuf.st_ino;

	current = calloc(1, PATH_MAX);
	*current++ = '/';

	// keep going until the root directory is hit
	for (;;) {

		if (chdir("..") == -1) {
			free(current - 1);
			close(cwd_save);
			return NULL;
		}

		if (stat(".", &statbuf) == -1) {
			free(current - 1);
			close(cwd_save);
			return NULL;
		}

		// root directory
		if (statbuf.st_ino == dir_inode) {
			break;
		}

		dirp = opendir(".");
		if (dirp == NULL) {
			free(current - 1);
			close(cwd_save);
			return NULL;
		}

		for (;;) {
			direntp = readdir(dirp);
			if (direntp == NULL) {
				free(current -1);
				close(cwd_save);
				return NULL;
			}

			if (direntp->d_ino == dir_inode) {

				len_c = strlen(current);
				len_d = strlen(direntp->d_name) + 1;

				// pathname exceeds PATH_MAX, don't buffer it
				if (len_c + len_d >= PATH_MAX) {
					free(current - 1);
					close(cwd_save);
					closedir(dirp);
					return NULL;
				}

				if (len_c  > 0) {
					tmp = calloc(1, len_d);
					memcpy(tmp, direntp->d_name, len_d - 1);
					tmp[len_d - 1] = '/';

					memmove(current + len_d, current, len_c);
					memcpy(current, tmp, len_d);
					free(tmp);
				} else {
					memcpy(current, direntp->d_name, len_d - 1);
				}

				closedir(dirp);
				dir_inode = statbuf.st_ino;
				break;
			}
		}
	}

	fchdir(cwd_save);
	close(cwd_save);

	return current - 1;
}


int main(int argc, char *argv[])
{
	char *cwd = t_getcwd();
	if (cwd) {
		printf("%s\n", cwd);
		free(cwd);
	}
	exit(EXIT_SUCCESS);
}