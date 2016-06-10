#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int i;
	int fd;
	int attr;
	int add;
	char *opt = NULL;

	if (argc < 3) {
		usageErr("%s filename [+options[-options]...]\n", argv[0]);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		errExit("open()");
	}

	// get existing file attributes
	if (ioctl(fd, FS_IOC_GETFLAGS, &attr) == -1) {
		errExit("ioctl()");
	}

	for (i = 2; i < argc; i++) {

		opt = argv[i];
		if (*opt == '+') {
			add = 1;
			opt++;
		} else if (*opt == '-') {
			add = 0;
			opt++;
		} else {
			add = 1;
		}

		while (*opt) {

			switch (*opt++) {
				case 'a':
					attr = add ? attr | FS_APPEND_FL : attr & FS_APPEND_FL ? attr ^ FS_APPEND_FL : attr;
					break;
				case 'c':
					attr = add ? attr | FS_COMPR_FL : attr & FS_COMPR_FL ? attr ^ FS_COMPR_FL : attr;
					break;
				case 'D':
					attr = add ? attr | FS_DIRSYNC_FL : attr & FS_DIRSYNC_FL ? attr ^ FS_DIRSYNC_FL : attr;
					break;
				case 'i':
					attr = add ? attr | FS_IMMUTABLE_FL : attr & FS_IMMUTABLE_FL ? attr ^ FS_IMMUTABLE_FL : attr;
					break;
				case 'j':
					attr = add ? attr | FS_JOURNAL_DATA_FL : attr & FS_JOURNAL_DATA_FL ? attr ^ FS_JOURNAL_DATA_FL : attr;
					break;
				case 'A':
					attr = add ? attr | FS_NOATIME_FL : attr & FS_NOATIME_FL ? attr ^ FS_NOATIME_FL : attr;
					break;
				case 'd':
					attr = add ? attr | FS_NODUMP_FL : attr & FS_NODUMP_FL ? attr ^ FS_NODUMP_FL : attr;
					break;
				case 't':
					attr = add ? attr | FS_NOTAIL_FL : attr & FS_NOTAIL_FL ? attr ^ FS_NOTAIL_FL : attr;
					break;
				case 's':
					attr = add ? attr | FS_SECRM_FL : attr & FS_SECRM_FL ? attr ^ FS_SECRM_FL : attr;
					break;
				case 'S':
					attr = add ? attr | FS_SYNC_FL : attr & FS_SYNC_FL ? attr ^ FS_SYNC_FL : attr;
					break;
				case 'T':
					attr = add ? attr | FS_TOPDIR_FL : attr & FS_TOPDIR_FL ? attr ^ FS_TOPDIR_FL : attr;
					break;
				case 'u':
					attr = add ? attr | FS_UNRM_FL : attr & FS_UNRM_FL ? attr ^ FS_UNRM_FL : attr;
					break;
				default:
					printf("invalid option: %c\n", *opt);
					break;
			}
		}
	}

	// set new file attributes
	if (ioctl(fd, FS_IOC_SETFLAGS, &attr) == -1) {
		errExit("ioctl()");
	}

	close(fd);

	exit(EXIT_SUCCESS);
}
