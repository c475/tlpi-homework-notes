#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"


struct FTW2 {
	int whatever;
};

int nftw(const char *dir, int (*func) (const char *pathname, const struct stat *sbuf, int type, struct FTW2 *ftwb), 
	int numfd, int flags)
{
	return 0;
}


int main(int argc, char *argv[])
{
	exit(EXIT_SUCCESS);
}
