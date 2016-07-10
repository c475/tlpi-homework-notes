#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int fd;

	if (mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
		errExit("mkdir()");
	}

	if (chdir("test") == -1) {
		errExit("chdir()");
	}

	fd = open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		errExit("mkdir()");
	}

	// this is where it goes wrong.
	// it creates a symlink to "myfile" and
	// "../mylink" will look for "myfile", not "test/myfile".
	// therefore it is a dangling link
	if (symlink("myfile", "../mylink") == -1) {
		errExit("symlink()");
	}

	// can't call chmod() on a dangling link, problem has nothing to do with chmod()
	if (chmod("../mylink", S_IRUSR) == -1) {
		errExit("chmod()");
	}

	exit(EXIT_SUCCESS);
}
