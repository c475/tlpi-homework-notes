#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/file_perms.h"
#include "../../lib/tlpi_hdr.h"

#define MYFILE "myfile"
#define MYDIR "mydir"

#define OPEN_SETTINGS	(O_RDWR  | O_CREAT | O_EXCL)
#define FILE_PERMS 		(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define DIR_PERMS		(S_IRWXU | S_IRWXG | S_IRWXO)
#define UMASK_SETTING	(S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH)


int main(int argc, char *argv[])
{
	int fd;
	struct stat sb;
	mode_t u;

	umask(UMASK_SETTING);

	fd = open(MYFILE, OPEN_SETTINGS, FILE_PERMS);
	if (fd == -1) {
		errExit("open()");
	}

	if (mkdir(MYDIR, DIR_PERMS) == -1) {
		errExit("mkdir-%s", MYDIR);
	}

	u = umask(0);

	if (stat(MYFILE, &sb) == -1) {
		errExit("stat-%s", MYFILE);
	}

	printf("Requested file permissions: %s\n",   filePermStr(FILE_PERMS, 0));
	printf("Process mask:               %s\n",   filePermStr(u, 0));
	printf("Actual file permissions:    %s\n\n", filePermStr(sb.st_mode, 0));

	if (stat(MYDIR, &sb) == -1) {
		errExit("stat-%s", MYDIR);
	}

	printf("Requested dir permissions:  %s\n",   filePermStr(DIR_PERMS, 0));
	printf("Process mask:               %s\n",   filePermStr(u, 0));
	printf("Actual dir perms:           %s\n",   filePermStr(sb.st_mode, 0));

	if (unlink(MYFILE) == -1) {
		errMsg("unlink-%s", MYFILE);
	}

	if (rmdir(MYDIR) == -1) {
		errMsg("rmdir-%s", MYDIR);
	}

	exit(EXIT_SUCCESS);
}
