#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"



// int alpha_string(const char *str)
// {
// 	while (*str++) {
// 		switch (*str) {
// 			case 'a'...'z':
// 				break;
// 			case 'A'...'Z':
// 				break;
// 			default:
// 				return 0;
// 		}
// 	}

// 	return 1;
// }


// int purge_dir(const char *dirname, const char *child)
// {
// 	DIR *dirp;
// 	struct dirent *direntp;
// 	char *childdir = NULL;
// 	size_t buffsize = 0;
// 	size_t ld = strlen(dirname);

// 	if (child) {
// 		dirname = child;
// 	}

// 	if (rmdir(dirname) == -1) {
// 		dirp = opendir(dirname);
// 		if (dirp == NULL) {
// 			return -1;
// 		}

// 		while ((direntp = readdir(dirp)) != NULL) {

// 			if (alpha_string(direntp->d_name)) {
// 				if (direntp->d_type == DT_DIR) {
// 					buffsize = ld + strlen(direntp->d_name) + 2;
// 					childdir = calloc(1, buffsize);
// 					snprintf(childdir, buffsize - 1, "%s")
// 					purge_dir(NULL, childdir);
// 				} else {
// 					unlink(direntp->d_name);
// 				}

// 			}
// 		}

// 		if (rmdir(dirname) == -1) {

// 		}
// 	}

// 	if (child) {
// 		free(child);
// 	}

// 	closedir(dirp);

// 	return 0;
// }


int main(int argc, char *argv[])
{
	int fd;
	DIR *dirp;
	struct dirent *direntp;
	char fbuff[512];

	if (argc < 2) {
		usageErr("%s dirname\n", argv[0]);
	}

	if (strlen(argv[1]) > 100) {
		fprintf(stderr, "dirname too long\n");
		exit(EXIT_FAILURE);
	}

	rmdir(argv[1]);

	printf("creating this new directory with rwx perms: %s\n", argv[1]);
	if (mkdir(argv[1], S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
		errExit("mkdir()");
	}

	memset(fbuff, 0, 512);
	snprintf(fbuff, 511, "%s/%s", argv[1], "testfile");

	unlink(fbuff);

	printf("creating testfile in new directory.\n");
	fd = open(fbuff, O_CREAT | O_EXCL);
	if (fd == -1) {
		errExit("open(): could not create test file");
	}

	close(fd);

	printf("changing directory permissions to rw-\n");
	if (chmod(argv[1], S_IRUSR | S_IWUSR) == -1) {
		errExit("chmod()");
	}

	dirp = opendir(argv[1]);
	if (dirp == NULL) {
		errExit("opendir()");
	}

	// demonstrate the directory can be scanned
	printf("You can still read directory contents:\n");
	while ((direntp = readdir(dirp)) != NULL) {
		printf("%s\t", direntp->d_name);
	}

	printf("\n");

	closedir(dirp);

	fd = open(fbuff, O_RDONLY);
	if (fd == -1) {
		errExit("But can no longer open files within it.");
	}

	exit(EXIT_FAILURE);
}
