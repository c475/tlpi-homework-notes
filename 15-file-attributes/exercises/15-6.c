#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int i;
	struct stat st;

	for (i = 1; i < argc; i++) {
		if (stat(argv[i], &st) == -1) {
			continue;
		}

		switch (st.st_mode & S_IFMT) {
			// is directory
			case S_IFDIR:
				st.st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
				st.st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
				chmod(argv[i], st.st_mode);
				break;

			// is regular file
			case S_IFREG:
				if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
					st.st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
				}

				st.st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
				chmod(argv[i], st.st_mode);
				break;

			default:
				break;
		}
	}

	exit(EXIT_SUCCESS);
}
