#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <fcntl.h>
#include <ftw.h>
#include "../../lib/tlpi_hdr.h"

/*

$ ./18-7 /
directories:         8.20%
files:               75.79%
character devices:   0.05%
block devices:       0.01%
symlinks:            15.95%
fifos:               0.00%
sockets:             0.00%

*/

struct nftw_num {
	float total;
	float num_dir;
	float num_file;
	float num_char;
	float num_block;
	float num_link;
	float num_fifo;
	float num_sock;
	float other;
};


struct nftw_num NUMS;


int perc(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
	switch (sbuf->st_mode & S_IFMT) {

		case S_IFREG:
			NUMS.num_file += 1;
			NUMS.total += 1;
			break;

		case S_IFDIR:
			NUMS.num_dir += 1;
			NUMS.total += 1;
			break;

		case S_IFCHR:
			NUMS.num_char += 1;
			NUMS.total += 1;
			break;

		case S_IFBLK:
			NUMS.num_block += 1;
			NUMS.total += 1;
			break;

		case S_IFLNK:
			NUMS.num_link += 1;
			NUMS.total += 1;
			break;

		case S_IFIFO:
			NUMS.num_fifo += 1;
			NUMS.total += 1;
			break;

		case S_IFSOCK:
			NUMS.num_sock += 1;
			NUMS.total += 1;
			break;

		default:
			NUMS.other += 1;
			NUMS.total += 1;
			break;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	memset(&NUMS, 0, sizeof(struct nftw_num));

	if (access(argc < 2 ? "." : argv[1], F_OK) == -1) {
		errExit("file or directory does not exist");
	}

	if (nftw(argc < 2 ? "." : argv[1], perc, 20, FTW_PHYS) == -1) {
		errExit("nftw()");
	}

	if (NUMS.total) {
		printf("directories        ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_dir / NUMS.total) * 100, (int)NUMS.num_dir);
		printf("files              ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_file / NUMS.total) * 100, (int)NUMS.num_file);
		printf("character devices  ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_char / NUMS.total) * 100, (int)NUMS.num_char);
		printf("block devices      ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_block / NUMS.total) * 100, (int)NUMS.num_block);
		printf("symlinks           ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_link / NUMS.total) * 100, (int)NUMS.num_link);
		printf("fifos:             ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_fifo / NUMS.total) * 100, (int)NUMS.num_fifo);
		printf("sockets:           ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_sock / NUMS.total) * 100, (int)NUMS.num_sock);
		printf("other:             ~ p: %.2f%%\t\tc: %d\n", (NUMS.other / NUMS.total) * 100, (int)NUMS.other);
	} else {
		printf("Empty directory.\n");
	}

	exit(EXIT_SUCCESS);
}
