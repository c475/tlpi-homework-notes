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

// struct nftw_num {
// 	float total;
// 	float num_dir;
// 	float num_file;
// 	float num_char;
// 	float num_block;
// 	float num_link;
// 	float num_fifo;
// 	float num_sock;
// 	float other;
// };


// struct nftw_num NUMS;

static long int count = 0;


int perc(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
	count += 1;
	// switch (sbuf->st_mode & S_IFMT) {

	// 	case S_IFREG:
	// 		NUMS.num_file += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	case S_IFDIR:
	// 		NUMS.num_dir += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	case S_IFCHR:
	// 		NUMS.num_char += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	case S_IFBLK:
	// 		NUMS.num_block += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	case S_IFLNK:
	// 		NUMS.num_link += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	case S_IFIFO:
	// 		NUMS.num_fifo += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	case S_IFSOCK:
	// 		NUMS.num_sock += 1;
	// 		NUMS.total += 1;
	// 		break;

	// 	default:
	// 		NUMS.other += 1;
	// 		NUMS.total += 1;
	// 		break;
	// }

	return 0;
}


/*
	Intertesting note:

	$ valgrind ./18-7 /

		...snip...

==21468== HEAP SUMMARY:
==21468==     in use at exit: 0 bytes in 0 blocks
==21468==   total heap usage: 47,811 allocs, 47,811 frees, 1,569,952,864 bytes allocated


	1,569,952,864 bytes allocated is a lot of memory (almost 1.5 GB)

	So, thinking FTW_CHDIR would maybe reduce memory footprint (no need for string allocation) I tested it:

	(with FTW_CHDIR)
	$ valgrind ./18-7 /


==21548== Conditional jump or move depends on uninitialised value(s)
==21548==    at 0x40100A: perc (18-7.c:39)
==21548==    by 0x4F3101E: process_entry (ftw.c:467)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F31DE1: ftw_startup (ftw.c:773)
==21548==    by 0x4012C3: main (18-7.c:110)
==21548== 
==21548== Conditional jump or move depends on uninitialised value(s)
==21548==    at 0x401015: perc (18-7.c:39)
==21548==    by 0x4F3101E: process_entry (ftw.c:467)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F31DE1: ftw_startup (ftw.c:773)
==21548==    by 0x4012C3: main (18-7.c:110)
==21548== 
==21548== Conditional jump or move depends on uninitialised value(s)
==21548==    at 0x40101C: perc (18-7.c:39)
==21548==    by 0x4F3101E: process_entry (ftw.c:467)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F31DE1: ftw_startup (ftw.c:773)
==21548==    by 0x4012C3: main (18-7.c:110)
==21548== 
==21548== Conditional jump or move depends on uninitialised value(s)
==21548==    at 0x401027: perc (18-7.c:39)
==21548==    by 0x4F3101E: process_entry (ftw.c:467)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F311C3: process_entry (ftw.c:464)
==21548==    by 0x4F31502: ftw_dir (ftw.c:547)
==21548==    by 0x4F31DE1: ftw_startup (ftw.c:773)
==21548==    by 0x4012C3: main (18-7.c:110)
==21548== 
directories        ~ p: 8.70%		c: 51004
files              ~ p: 73.33%		c: 429747
character devices  ~ p: 0.04%		c: 238
block devices      ~ p: 0.01%		c: 30
symlinks           ~ p: 17.91%		c: 104970
fifos:             ~ p: 0.00%		c: 12
sockets:           ~ p: 0.00%		c: 25
other:             ~ p: 0.00%		c: 0
==21548== 
==21548== HEAP SUMMARY:
==21548==     in use at exit: 0 bytes in 0 blocks
==21548==   total heap usage: 51,004 allocs, 51,004 frees, 1,674,734,352 bytes allocated
==21548== 
==21548== All heap blocks were freed -- no leaks are possible
==21548== 
==21548== For counts of detected and suppressed errors, rerun with: -v
==21548== Use --track-origins=yes to see where uninitialised values come from
==21548== ERROR SUMMARY: 4 errors from 4 contexts (suppressed: 0 from 0)


	Seems even worse, and possibly a bug or risky behavior somewhere in nftw().

	Also just for fun:


$ valgrind wget
==21560== Memcheck, a memory error detector
==21560== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==21560== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==21560== Command: wget
==21560== 
wget: missing URL
Usage: wget [OPTION]... [URL]...

Try `wget --help' for more options.
==21560== 
==21560== HEAP SUMMARY:
==21560==     in use at exit: 33 bytes in 2 blocks
==21560==   total heap usage: 59 allocs, 57 frees, 10,576 bytes allocated
==21560== 
==21560== LEAK SUMMARY:
==21560==    definitely lost: 0 bytes in 0 blocks
==21560==    indirectly lost: 0 bytes in 0 blocks
==21560==      possibly lost: 0 bytes in 0 blocks
==21560==    still reachable: 33 bytes in 2 blocks
==21560==         suppressed: 0 bytes in 0 blocks
==21560== Rerun with --leak-check=full to see details of leaked memory
==21560== 
==21560== For counts of detected and suppressed errors, rerun with: -v
==21560== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

*/

int main(int argc, char *argv[])
{
	// memset(&NUMS, 0, sizeof(struct nftw_num));

	// if (access(argc < 2 ? "." : argv[1], F_OK) == -1) {
	// 	errExit("file or directory does not exist");
	// }

	if (nftw(argc < 2 ? "." : argv[1], perc, 20, FTW_PHYS) == -1) {
		errExit("nftw()");
	}

	printf("%ld\n", count);

	// if (NUMS.total) {
	// 	printf("directories        ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_dir / NUMS.total) * 100, (int)NUMS.num_dir);
	// 	printf("files              ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_file / NUMS.total) * 100, (int)NUMS.num_file);
	// 	printf("character devices  ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_char / NUMS.total) * 100, (int)NUMS.num_char);
	// 	printf("block devices      ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_block / NUMS.total) * 100, (int)NUMS.num_block);
	// 	printf("symlinks           ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_link / NUMS.total) * 100, (int)NUMS.num_link);
	// 	printf("fifos:             ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_fifo / NUMS.total) * 100, (int)NUMS.num_fifo);
	// 	printf("sockets:           ~ p: %.2f%%\t\tc: %d\n", (NUMS.num_sock / NUMS.total) * 100, (int)NUMS.num_sock);
	// 	printf("other:             ~ p: %.2f%%\t\tc: %d\n", (NUMS.other / NUMS.total) * 100, (int)NUMS.other);
	// } else {
	// 	printf("Empty directory.\n");
	// }

	exit(EXIT_SUCCESS);
}
