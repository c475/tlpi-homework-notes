#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include "../../lib/tlpi_hdr.h"


/*
	Write a program that measures the time required to create and then remove a
	large number of 1-byte files from a single directory. The program should create
	files with names of the form xNNNNNN , where NNNNNN is replaced by a random six-digit
	number. The files should be created in the random order in which their names are
	generated, and then deleted in increasing numerical order (i.e., an order that is
	different from that in which they were created). The number of files (NF) and the
	directory in which they are to be created should be specifiable on the command
	line. Measure the times required for different values of NF (e.g., in the range from
	1000 to 20,000) and for different file systems (e.g., ext2, ext3, and XFS). What
	patterns do you observe on each file system as NF increases? How do the various
	file systems compare? Do the results change if the files are created in increasing
	numerical order (x000000 , x000001 , x0000002 , and so on) and then deleted in the same
	order? If so, what do you think the reason(s) might be? Again, do the results vary
	across file-system types?
*/

typedef struct FileName {
	char path[PATH_MAX]; 	// path to file
	int realnum; 			// number corresponding to pathname for easier sorting
	int status; 			// status 0: exists, status 1: unlinked
	struct FileName *prev;
	struct FileName *next;
} FileName;


FileName *rewindFileName(FileName *fn)
{
	FileName *rw = fn;

	if (!rw) {
		return NULL;
	}

	while (rw->prev) {
		rw = rw->prev;
	}

	return rw;
}



int isDuplicate(FileName *fn, char *num)
{
	FileName *fnp = rewindFileName(fn);
	int n = atoi(num);
	
	while (fnp) {
		if (fnp->realnum == n) {
			return 1;
		}

		fnp = fnp->next;
	}

	return 0;
}


FileName *generateFilename(FileName *fn, const char *dir)
{
	int num = rand();
	FileName *newfn = calloc(1, sizeof(FileName));
	char name[8] = {0};

	if (fn != NULL) {
		fn->next = newfn;
		newfn->prev = fn;
		newfn->next = NULL;
	} else {
		newfn->next = NULL;
		newfn->prev = NULL;
	}

	snprintf(name, 8, "x%d", num);

	while (isDuplicate(fn, name + 1)) {
		num = rand();
		snprintf(name, 8, "x%d", num);
	}

	snprintf(newfn->path, PATH_MAX - 1, "%s/%s", dir, name);

	newfn->status = 0;
	newfn->realnum = atoi(name + 1);

	return newfn;
}


FileName *getMinFn(FileName *fn)
{
	FileName *min = NULL;
	FileName *fnp = rewindFileName(fn);

	while (fnp) {

		if (fnp->status == 0) {

			if (min == NULL || (fnp->realnum < min->realnum)) {
				min = fnp;
			}

		}

		fnp = fnp->next;
	}

	return min;
}


void freeAll(FileName *fn)
{
	FileName *fnp = rewindFileName(fn);
	FileName *node = NULL;

	while (fnp) {
		node = fnp;
		fnp = fnp->next;
		free(node);
	}
}


void removeAll(FileName *fn)
{
	int rm;
	FileName *min = getMinFn(fn);

	do {

		rm = remove(min->path);
		if (rm == -1) {
			errExit("remove()");
		}

		min->status = 1;
		min = getMinFn(min);

	} while (min != NULL);
}


int main(int argc, char *argv[])
{
	int fd;
	char *dirname;
	int numFiles = 10;
	mode_t open_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	FileName *filename = NULL;

	if (argc < 2) {
		usageErr("./program dir [numfiles]");
	}

	if (argc >= 3) {
		if (isNumericString(argv[2])) {
			numFiles = atoi(argv[2]);
		} else {
			errExit("[numfiles] argument is not numeric");
		}
	}

	dirname = argv[1];

	srand(time(NULL));

	while (numFiles--) {

		filename = generateFilename(filename == NULL ? NULL : filename, dirname);

		fd = open(filename->path, O_CREAT | O_WRONLY, open_flags);
		if (fd == -1) {
			errExit("open()");
		}

		write(fd, "a", 1);
		close(fd);
	}

	removeAll(filename);
	freeAll(filename);

	exit(EXIT_SUCCESS);
}
