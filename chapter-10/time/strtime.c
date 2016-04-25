#define _X_OPEN_SOURCE
#define _GNU_SOURCE
#include <time.h>
#include <locale.h>
#include "../../lib/tlpi_hdr.h"


#define SBUF_SIZE 1024


int main(int argc, char *argv[])
{
	struct tm tm;
	char sbuf[SBUF_SIZE];
	char *ofmt;

	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s input-date-time in-format [out-format]\n", argv[0]);
	}

	if (setlocale(LC_ALL, "") == NULL) {
		errExit("setlocale");
	}

	memset(&tm, 0, sizeof(struct tm));

	if (strptime(argv[1], argv[2], &tm) == NULL) {
		fatal("strptime");
	}

	// tell mktime() to determine dst for us
	tm.tm_isdst = -1;

	printf("epoch time: %ld\n", (long)mktime(&tm));

	ofmt = (argc > 3) ? argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
	if (strftime(sbuf, SBUF_SIZE, ofmt, &tm) == 0) {
		fatal("strftime returned 0");
	}

	printf("strftime() yields: %s\n", sbuf);

	exit(EXIT_SUCCESS);
}
