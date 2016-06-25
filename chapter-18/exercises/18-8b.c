#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"


struct FTW2 {
	int base;    // offset to basename part of pathname
	int level;   // depth of file within tree traversal
};


struct Book {
	dev_t current_mount;
	struct FTW2 state;
};


// typedef struct FTW_BOOKKEEPING {

// 	int active_size;
// 	int resting_size;
// 	int active_fd[1024];
// 	int resting_fd[1024];

// } FTW_BOOKKEEPING;


// static FTW_BOOKKEEPING 	FTW_BOOK  = {0};
// static struct FTW2 	    FTW_LEVEL = {0};

/*
	int typeflag

		FTW_D: this is a directory

		FTW_DNR: this is a directory that could not be read

		FTW_DP: Doing a postorder traversal (FTW_DEPTH), 
			and this is a directory whose contents have already been processed.

		FTW_F: File of any type other than a directory or symbolic link.

		FTW_NS: Calling stat() failed (probably permissions). "statbuf" is undefined.

		FTW_SL: Symbolic link. Only passed in if using FTW_PHYS flag.

		FTW_SLN: Dangling symbolic link. Only passed in if FTW_PHYS is NOT used.
*/
#define FTW_D 				0x01
#define FTW_DNR 			0x02
#define FTW_DP 				0x04
#define FTW_F 				0x08
#define FTW_NS 				0x10
#define FTW_SL 				0x20
#define FTW_SLN 			0x80


/*
	FLAGS:

		FTW_CHDIR

			Do a chdir() into each directory before processing its contents.

		FTW_DEPTH

			Process all files before processing the containing directory with func()

		FTW_MOUNT

			Do not cross over into another filesystem.

		FTW_PHYS

			Do not dereference symbolic links. Passes typeflag as FTW_SL to func() instead.
*/
#define FTW_CHDIR 			0x2000
#define FTW_DEPTH 	    	0x4000
#define FTW_MOUNT 			0x8000
#define FTW_PHYS 			0x10000


/*
	FTW_ACTIONRETVAL:

	Changes behavior of the return value from func(). func() can return the following in this case:

		FTW_CONTINUE

			Continue processing

		FTW_SKIP_SIBLINGS

			Don't process any further in the current directory

		FTW_SKIP_SUBTREE

			If pathname is a directory, don;t call func() for entries in this directory. I imagine this works better when not using FTW_DEPTH (postorder traversal)

		FTW_STOP

			Don't process any further entries in this directory
*/
#define FTW_ACTIONRETVAL 	0x100
#define FTW_CONTINUE 		0x200
#define FTW_SKIP_SIBLINGS 	0x400
#define FTW_SKIP_SUBTREE 	0x800
#define FTW_STOP 			0x1000


int somefunc(const char *pathname, const struct stat *sb, int type, struct FTW2 *ftwp)
{
	switch (type) {

		case FTW_D:
			printf("FTW_D: %s\n", pathname);
			break;

		case FTW_DNR:
			printf("FTW_DNR: %s\n", pathname);
			exit(EXIT_SUCCESS);
			break;

		case FTW_DP:
			printf("FTW_DP: %s\n", pathname);
			break;

		case FTW_F:
			printf("FTW_F: %s\n", pathname);
			break;

		case FTW_NS:
			printf("FTW_NS: %s\n", pathname);
			break;

		case FTW_SL:
			printf("FTW_SL: %s\n", pathname);
			return -1;

		case FTW_SLN:
			printf("FTW_SLN: %s\n", pathname);
			break;
	}

	return 0;
}


static inline int is_dir(const char *dirname, struct stat *statbuf)
{
	struct stat statbuf2;

	if (dirname == NULL && statbuf == NULL) {
		return 0;
	}

	if (statbuf == NULL) {
		if (stat(dirname, &statbuf2) == -1) {
			return 0;
		}

		return (statbuf2.st_mode & S_IFMT) == S_IFDIR;
	}

	return (statbuf->st_mode & S_IFMT) == S_IFDIR;
}

/*
	int typeflag

		FTW_D: this is a directory

		FTW_DNR: this is a directory that could not be read

		FTW_DP: Doing a postorder traversal (FTW_DEPTH), 
			and this is a directory whose contents have already been processed.

		FTW_F: File of any type other than a directory or symbolic link.

		FTW_NS: Calling stat() failed (probably permissions). "statbuf" is undefined.

		FTW_SL: Symbolic link. Only passed in if using FTW_PHYS flag.

		FTW_SLN: Dangling symbolic link. Only passed in if FTW_PHYS is NOT used.
*/
#define FTW_D 				0x01
#define FTW_DNR 			0x02
#define FTW_DP 				0x04
#define FTW_F 				0x08
#define FTW_NS 				0x10
#define FTW_SL 				0x20
#define FTW_SLN 			0x80


static inline int getstat(const char *pathname, struct stat *statbuf, int flags)
{
	if (flags & FTW_PHYS) {
		if (lstat(pathname, statbuf) == -1) {
			return -1;
		}
	} else {
		if (stat(pathname, statbuf) == -1) {
			return -1;
		}
	}

	return 0;
}


static inline int ftw_file_type(const char *pathname, const struct stat *statbuf)
{
	switch (statbuf->st_mode & S_IFMT) {

		case S_IFLNK:
			return access(pathname, F_OK) != -1 ? FTW_SL : FTW_SLN;

		case S_IFDIR:
			return FTW_D;

		default:
			return FTW_F;

	}

	return FTW_F;
}


// might need flags to check for crossing mount points
static inline int ftw_skip(const char *pathname)
{
	for (; *pathname; pathname++) {
		if (strcmp(pathname, "/dev/fd") == 0) {
			return 1;
		}

		if (strcmp(pathname, "/proc") == 0) {
			return 1;
		}

		if (strcmp(pathname, "/sys") == 0) {
			return 1;
		}

		if (strcmp(pathname, "/usr/bin/X11") == 0) {
			return 1;
		}
	}

	return 0;
}


int t_nftw(const char *dirname, int (*func) (const char *pathname, const struct stat *sbuf, int type, struct FTW2 *ftwb), 
	int numfd, int flags)
{
	DIR *dirp;
	struct stat statbuf;
	struct dirent *direntp;
	size_t path_len;
	char *path;
	int ftype;
	int func_val;
	int nftw_val;
	int skip_subtree;

	path = NULL;
	skip_subtree = 0;

	memset(&statbuf, 0, sizeof(struct stat));

	if (ftw_skip(dirname)) {
		return 0;
	}

	if (!is_dir(dirname, NULL)) {
		return 0;
	}

	dirp = opendir(dirname);
	// can't read directory
	if (dirp == NULL) {
		func(dirname, NULL, FTW_DNR, NULL);
		return 0;
	}

	for (;;) {

		direntp = readdir(dirp);
		if (direntp == NULL) {
			break;
		}

		if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) {
			continue;
		}

		path_len = strlen(dirname) + strlen(direntp->d_name) + 2;
		path = calloc(1, path_len);
		if (path == NULL) {
			return 0;
		}

		snprintf(path, path_len, strcmp(dirname, "/") == 0 ? "%s%s" : "%s/%s", dirname, direntp->d_name);

		if (getstat(path, &statbuf, flags) == -1) {
			ftype = FTW_NS;
		} else {
			ftype = ftw_file_type(path, &statbuf);
		}

		func_val = func(path, &statbuf, ftype, 0);


		if ((flags & FTW_ACTIONRETVAL) && func_val != FTW_CONTINUE) {

			if (func_val == FTW_STOP) {
				free(path);
				closedir(dirp);
				return FTW_STOP;
			}

			if (func_val == FTW_SKIP_SIBLINGS) {
				free(path);
				break;
			}

			if (func_val == FTW_SKIP_SUBTREE) {
				skip_subtree = 1;
			}

		} else {

			if (func_val != 0) {
				free(path);
				closedir(dirp);
				return func_val;
			}

		}

		if (is_dir(NULL, &statbuf)) {

			if (skip_subtree == 0) {

				nftw_val = t_nftw(path, func, numfd, flags);

				if (nftw_val != 0) {
					free(path);
					closedir(dirp);
					return nftw_val;
				}
			}
		}

		free(path);
	}

	closedir(dirp);

	return 0;
}


int main(int argc, char *argv[])
{
	t_nftw(argv[1], somefunc, 10, FTW_PHYS);
	exit(EXIT_SUCCESS);
}
