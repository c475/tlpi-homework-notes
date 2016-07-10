#include <unistd.h>
#include <sys/stat.h>


int eaccess(const char *pathname, mode_t mode)
{
	uid_t uid = geteuid();
	gid_t gid = getegid();

	struct stat st;

	if (stat(pathname, &st) == -1) {
		return -1;
	}

	if (uid == st.st_uid || gid == st.st_gid) {
		return 1;
	}

	if (st.st_mode & mode) {
		return 1;
	}

	return 0;
}


// driver for eaccess()
int main(int argc, char *argv[])
{
	/*
		uw, ur, ux
		gw, gr, gx
		ow, or, ox
	*/

	int i;
	mode_t flags = 0;

	if (argc < 2) {
		usageErr("%s filename [perm...]\n\tuw, ur, ux\n\tgw, gr, gx\n\tow, or, ox", argv[0]);
	}

	int i = 2;
	if (argc >= 3) {
		for (i = 2; i < argc; i++) {
			if (strcmp("uw", argv[i]) == 0) {
				flags |= S_IWUSR;
			} else if (strcmp("ur", argv[i]) == 0) {
				flags |= S_IRUSR;
			} else if (strcmp("ux", argv[i]) == 0) {
				flags |= S_IXUSR;
			} else if (strcmp("gw", argv[i]) == 0) {
				flags |= S_IWGRP;
			} else if (strcmp("gr", argv[i]) == 0) {
				flags |= S_IRGRP;
			} else if (strcmp("gx", argv[i]) == 0) {
				flags |= S_IXGRP;
			} else if (strcmp("ow", argv[i]) == 0) {
				flags |= S_IWOTH;
			} else if (strcmp("or", argv[i]) == 0) {
				flags |= S_IROTH;
			} else if (strcmp("ox", argv[i]) == 0) {
				flags |= S_IXOTH;
			}
		}
	}

	if (access(argv[1], flags)) {
		printf("access granted to %s\n", argv[1]);
	} else {
		printf("permission denied\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
