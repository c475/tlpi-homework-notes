#include <pwd.h>
#include <grp.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	uid_t uid;
	gid_t gid;
	int j;
	Boolean errFnd;

	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		usageErr(
			"%s owner group [file...]\n"
			"Owner of group can be '-', meaning leave unchanged\n",
			argv[0]
		);
	}

	if (strcmp(argv[1], "-") == 0) {
		uid = -1;
	} else {
		uid = userIdFromName(argv[1]);
		if (uid == -1) {
			fatal("no such user (%s)\n", argv[1]);
		}
	}

	if (strcmp(argv[2], "-") == 0) {
		gid = -1;
	} else {
		gid = groupIdFromName(argv[2]);
		if (gid == -1) {
			fatal("No group (%s)", argv[2]);
		}
	}

	errFnd = FALSE;
	for (j = 3; j < argc; j++) {
		if (chown(argv[j], uid, gid) == -1) {
			errMsg("chown: %s", argv[j]);
			errFnd = TRUE;
		}
	}

	exit(errFnd ? EXIT_FAILURE : EXIT_SUCCESS);
}
