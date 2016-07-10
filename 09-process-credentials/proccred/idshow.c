#define _GNU_SOURCE
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>

// userNameFromId() and groupNameFromId()
#include "../../chapter-08/users_groups/ugid_functions.h"
#include "../../lib/tlpi_hdr.h"

#define SG_SIZE (NGROUPS_MAX + 1)


/*
	At least the chapter ends with a great example of everything in it.
	Permissions are very complicated.
*/


int main(int argc, char *argv[])
{
	uid_t ruid, euid, suid, fsuid;
	gid_t rgid, egid, sgid, fsgid;
	gid_t suppGroups[SG_SIZE];
	int numGroups, j;
	char *p;

	if (getresuid(&ruid, &euid, &suid) == -1) {
		errExit("getresuid");
	}

	if (getresgid(&rgid, &egid, &sgid) == -1) {
		errExit("getresgid");
	}

	/*
		Attempts to change the file-system ids are always ignored
		for unprivileged processes, but even so, the following
		calls return the current file-system IDs
	*/

	fsuid = setfsuid(0);
	fsgid = setfsgid(0);


	printf("UID: ");

	p = userNameFromId(ruid);
	printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long)ruid);

	p = userNameFromId(euid);
	printf("eff=%s (%ld); ", (p == NULL) ? "???" : p, (long)euid);

	p = userNameFromId(suid);
	printf("saved=%s (%ld); ", (p == NULL) ? "???" : p, (long)suid);

	p = userNameFromId(fsuid);
	printf("fs=%s (%ld); ", (p == NULL) ? "???" : p, (long)fsuid);

	printf("\n");


	printf("GID: ");

	p = groupNameFromId(rgid);
	printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long)rgid);

	p = groupNameFromId(egid);
	printf("eff=%s (%ld); ", (p == NULL) ? "???" : p, (long)egid);

	p = groupNameFromId(sgid);
	printf("saved=%s (%ld); ", (p == NULL) ? "???" : p, (long)sgid);

	p = groupNameFromId(fsgid);
	printf("fs=%s (%ld); ", (p == NULL) ? "???" : p, (long)fsgid);

	printf("\n");


	numGroups = getgroups(SG_SIZE, suppGroups);
	if (numGroups == -1) {
		errExit("getgroups");
	}

	printf("Supplementary groups (%d): ", numGroups);
	for (j = 0; j < numGroups; j++) {
		p = groupNameFromId(suppGroups[j]);
		printf("%s (%ld) ", (p == NULL) ? "???" : p, (long)suppGroups[j]);
	}

	printf("\n");

	exit(EXIT_SUCCESS);
}
