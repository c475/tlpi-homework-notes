#include <fcntl.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"

#define PATH_MAX 1024
#define CONTENT_MAX 1024*4


int main(int argc, char *argv[])
{
	int fd;
	DIR *dp;
	struct dirent *dir;

	char pathname[PATH_MAX] = {0};
	char content[CONTENT_MAX] = {0};


	dp = opendir("")

	// readlink(pathname, content, CONTENT_MAX-1)