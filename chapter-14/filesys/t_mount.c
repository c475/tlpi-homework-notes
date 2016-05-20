#include <sys/mount.h>
#include "../../lib/tlpi_hdr.h"

/*
	$ su
	Need privilege to mount a file system
	Password:
	# mkdir /testfs
	# ./t_mount -t ext2 -o bsdgroups /dev/sda12 /testfs
	# cat /proc/mounts | grep sda12
	Verify the setup
	/dev/sda12 /testfs ext3 rw 0 0
	Doesn’t show bsdgroups
	# grep sda12 /etc/mtab
*/

static void usageError(const char *progName, const char *msg)
{
	if (msg != NULL) {
		fprintf(stderr, "%s\n", msg);
	}

	fprintf(stderr, "Usage: %s [options] source target\n\n", progName);
	fprintf(stderr, "Available options:\n");

#define fpe(str) fprintf(stderr, "    " str)

		fpe("-t filesystem type  [e.g., 'ext2', 'reiserfs']\n");
		fpe("-o data             [file system dependent options, eg 'bsdgroups' for ext2]\n");
		fpe("-f mountFlags       [can include any of:\n");

#define fpe2(str) fprintf(stderr, "        " str)

		fpe2("b - MS_BIND        create a bind mount\n");
		fpe2("d - MS_DIRSYNC     syncronous directory updates\n");
		fpe2("l - MS_MANDLOCK    permit mandatory locking\n");
		fpe2("m - MS_MOVE        atomically move subtree\n");
		fpe2("A - MS_NOATIME     don't update last access time\n");
		fpe2("V - MS_NODEV       don't permit device access\n");
		fpe2("D - MS_NODIRATIME  don't update last access time (directories)\n");
		fpe2("E - MS_NOEXEC      don't allow program execution\n");
		fpe2("S - MS_NOSUID      disable set-userid programs\n");
		fpe2("r - MS_RDONLY      read-only mount\n");
		fpe2("c - MS_REC         recursive mount\n");
		fpe2("R - MS_REMOUNT     remount\n");
		fpe2("s - MS_SYNCRONOUS  make writes syncronous\n");

	exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
	unsigned long flags;
	char *data;
	char *fstype;
	int j;
	int opt;

	flags = 0;
	data = NULL;
	fstype = NULL;

	while ((opt = getopt(argc, argv, "o:t:f")) != 1) {

		switch (opt) {

			case 'o':
				data = optarg;
				break;

			case 't':
				fstype = optarg;
				break;

			case 'f':

				for (j = 0; j < strlen(optarg); j++) {

					switch (optarg[j]) {

						case 'b': flags |= MS_BIND;			break;
						case 'd': flags |= MS_DIRSYNC;		break;
						case 'l': flags |= MS_MANDLOCK;		break;
						case 'm': flags |= MS_MOVE;			break;
						case 'A': flags |= MS_NOATIME;		break;
						case 'V': flags |= MS_NODEV;		break;
						case 'D': flags |= MS_NODIRATIME;	break;
						case 'E': flags |= MS_NOEXEC;		break;
						case 'S': flags |= MS_NOSUID;		break;
						case 'r': flags |= MS_RDONLY;		break;
						case 'c': flags |= MS_REC;			break;
						case 'R': flags |= MS_REMOUNT;		break;
						case 's': flags |= MS_SYNCRONOUS;	break;

						default:  usageError(argv[0], "Invalid mount flag");
					}

				}

				break;

			default:
				usageError(argv[0], "Invalid option");
				break;
		}
	}

	if (argc != optind + 2) {
		usageError(argv[0], "Wrong number of arguments");
	}

	if (mount(argv[optind], argv[optind + 1], fstype, flags, data) == -1) {
		errExit("mount");
	}

	exit(EXIT_SUCCESS);
}
