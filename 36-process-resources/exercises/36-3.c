#include <sys/resource.h>
#include "../../lib/tlpi_hdr.h"


// RLIMIT_AS 			Process virtual memory size (bytes)
// RLIMIT_CORE			Core file size (bytes)
// RLIMIT_CPU 			CPU time (seconds)
// RLIMIT_DATA 			Process data segment (bytes)
// RLIMIT_FSIZE 		File size (bytes)
// RLIMIT_MEMLOCK 		Locked memory (bytes)
// RLIMIT_MSGQUEUE 		Bytes allocated for POSIX message queues for real user ID
// RLIMIT_NICE 			Nice value (since Linux 2.6.12)
// RLIMIT_NOFILE 		Maximum file descriptor number plus one
// RLIMIT_NPROC 		Number of processes for real user ID
// RLIMIT_RSS 			Resident set size (bytes; not implemented)
// RLIMIT_RTPRIO 		Realtime scheduling priority (since Linux 2.6.12)
// RLIMIT_RTTIME 		Realtime CPU time (microseconds; since Linux 2.6.25)
// RLIMIT_SIGPENDING 	Number of queued signals for real user ID (since
// RLIMIT_STACK 		Size of stack segment (bytes)
int main(int argc, char *argv[])
{
	int selection;
	int soft;
	int resource;
	struct rlimit limit;

	if (argc < 3 && strcmp(argv[1], "--help") == 0) {
		usageErr("%s resource-number soft-limit\n");
	}

	selection = getLong(argv[1], 0, "selection");
	soft = getLong(argv[2], 0, "soft");

	limit.rlim_cur = soft;
	limit.rlim_max = soft;

	switch (selection) {

		case 1:
			resource = RLIMIT_AS;
			break;

		case 2:
			resource = RLIMIT_CORE;
			break;

		case 3:
			resource = RLIMIT_CPU;
			break;

		case 4:
			resource = RLIMIT_DATA;
			break;

		case 5:
			resource = RLIMIT_FSIZE;
			break;

		case 6:
			resource = RLIMIT_MEMLOCK;
			break;

		case 7:
			resource = RLIMIT_MSGQUEUE;
			break;

		case 8:
			resource = RLIMIT_NICE;
			break;

		case 9:
			resource = RLIMIT_NOFILE;
			break;

		case 10:
			resource = RLIMIT_NPROC;
			break;

		case 11:
			resource = RLIMIT_RSS;
			break;

		case 12:
			resource = RLIMIT_RTPRIO;
			break;

		case 13:
			resource = RLIMIT_RTTIME;
			break;

		case 14:
			resource = RLIMIT_SIGPENDING;
			break;

		case 15:
			resource = RLIMIT_STACK;
			break;

		default:
			usageErr("Invalid resource number\n");
	}

	printRlimit("BEFORE LIMIT: ", resource);

	if (setrlimit(resource, &limit) == -1) {
		errExit("setrlimit");
	}

	printRlimit("AFTER LIMIT: ", resource);

	exit(EXIT_SUCCESS);
}
