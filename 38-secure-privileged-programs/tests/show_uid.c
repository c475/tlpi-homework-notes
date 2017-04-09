#define _GNU_SOURCE
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	uid_t uid, euid, suid;
	gid_t gid, egid, sgid;

	if (getresuid(&uid, &euid, &suid) == -1) {
		errExit("getresuid");
	}

	if (getresgid(&gid, &egid, &sgid) == -1) {
		errExit("getresgid");
	}

	printf("UID:  %ld\n", (long)uid);
	printf("EUID: %ld\n", (long)euid);
	printf("SUID: %ld\n", (long)suid);

	printf("GID:  %ld\n", (long)gid);
	printf("EGID: %ld\n", (long)egid);
	printf("SGID: %ld\n", (long)sgid);

	exit(EXIT_SUCCESS);
}
