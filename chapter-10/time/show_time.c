#include <time.h>
#include <locale.h>
#include "../../lib/tlpi_hdr.h"

#define BUF_SIZE 200


int main(int argc, char *argv[])
{
	time_t t;
	struct tm *loc;
	char buf[BUF_SIZE];

	if (setlocale(LC_ALL, "") == NULL) {
		errExit("setlocale");
	}

	t = time(NULL);

	printf("ctime() of time() value is: %s\n", ctime(&t));

	loc = localtime(&t);
	if (loc == NULL) {
		errExit("localtime");
	}

	printf("asctime() of local time is: %s\n", asctime(loc));

	if (strftime(buf, BUF_SIZE, "%A, %d %B %Y, %H:%M:%S %Z", loc) == 0) {
		fatal("strftime returned 0");
	}

	printf("strftime() of local time is: %s\n", buf);

	exit(EXIT_SUCCESS);
}
