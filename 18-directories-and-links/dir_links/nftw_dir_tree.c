#define _XOPEN_SOURCE 600  // get nftw() and S_IFSOCK declarations...

#include <ftw.h>
#include "../../lib/tlpi_hdr.h"


static void usageError(const char *progName, const char *msg)
{
	if (msg != NULL) {
		fprintf(stderr, "%s\n", msg);
	}

	fprintf(stderr, "Usage: %s [-d] [-m] [-p] [directory-path]\n", progName);
	fprintf(stderr, "\t-d Use FTW_DEPTH flag\n");
	fprintf(stderr, "\t-m Use FTW_MOUNT flag\n");
	fprintf(stderr, "\t-p Use FTW_PHYS flag\n");
	exit(EXIT_FAILURE);
}


static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
	// stat file type
	switch (sbuf->st_mode & S_IFMT) {

		case S_IFREG:
			printf("-");
			break;

		case S_IFDIR:
			printf("d");
			break;

		case S_IFCHR:
			printf("c");
			break;

		case S_IFBLK:
			printf("b");
			break;

		case S_IFLNK:
			printf("l");
			break;

		case S_IFIFO:
			printf("p");
			break;

		case S_IFSOCK:
			printf("s");
			break;

		// should never happen on linux
		default:
			printf("?");
			break;
	}

	// what type of file are we dealing with (nftw perspective)
	switch (type) {

		case FTW_D:
			printf(" D  ");
			break;

		case FTW_DNR:
			printf(" DNR");
			break;

		case FTW_DP:
			printf(" DP ");
			break;

		case FTW_F:
			printf(" F  ");
			break;

		case FTW_SL:
			printf(" SL ");
			break;

		case FTW_SLN:
			printf(" SLN");
			break;

		case FTW_NS:
			printf(" NS ");
			break;

		default:
			printf("   ");
			break;
	}

	// if stat() was able to be called on the file/directory
	if (type != FTW_NS) {
		printf("%7ld ", (long)sbuf->st_ino);
	} else {
		printf("       ");
	}

	// indent suitably
	printf(" %*s", 4 * ftwb->level, "");
	// print basename
	printf("%s\n", &pathname[ftwb->base]);

	return 0;
}


int main(int argc, char *argv[])
{
	int flags;
	int opt;

	flags = 0;

	while ((opt = getopt(argc, argv, "dmp")) != -1) {

		switch (opt) {

			case 'd':
				flags |= FTW_DEPTH;
				break;

			case 'm':
				flags |= FTW_MOUNT;
				break;

			case 'p':
				flags |= FTW_PHYS;
				break;

			default:
				usageError(argv[0], NULL);
				break;
		}

	}

	if (argc > optind + 1) {
		usageError(argv[0], NULL);
	}

	if (nftw(argc > optind ? argv[optind] : ".", dirTree, 10, flags) == -1) {
		perror("nftw()");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
