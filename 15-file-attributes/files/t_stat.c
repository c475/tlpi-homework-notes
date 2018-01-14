#define _DEFAULT_SOURCE // for major() and minor()
#include <sys/types.h> // some types in struct stat
#include <sys/stat.h>
#include <time.h>
#include <sys/sysmacros.h>
#include "../../lib/file_perms.h"
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	struct stat sb;

	// true if -l specified (i.e., use lstat)
	Boolean statLink;
	// location of filename argument in argv[]
	int fname;

	// simple parsing for -l
	statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;

	fname = statLink ? 2 : 1;

	if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
		usageErr("%s [-l] file\n    -l = use lstat() instead of stat()\n", argv[0]);
	}

	if (statLink) {
		if (lstat(argv[fname], &sb) == -1) {
			errExit("lstat()");
		}
	} else {
		if (stat(argv[fname], &sb) == -1) {
			errExit("stat()");
		}
	}

	displayStatInfo(&sb);

	exit(EXIT_SUCCESS);
}
