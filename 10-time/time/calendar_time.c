#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include "../../lib/tlpi_hdr.h"

#define SECONDS_IN_TROPICAL_YEAR (365.24219 * 24 * 60 * 60)


int main(int argc, char *argv[])
{
	time_t t;
	struct tm *gmp, *locp;
	struct tm gm, loc;
	struct timeval tv;


	t = time(NULL);

	printf("Seconds since epoch: %ld\n", (long)t);
	printf("Abouf %6.3f years\n\n", t / SECONDS_IN_TROPICAL_YEAR);

	if (gettimeofday(&tv, NULL) == -1) {
		errExit("gettimeofday");
	}

	printf(
		"gettimeofday() returned %ld secs and %ld microsec\n\n", 
		(long)tv.tv_sec,
		(long)tv.tv_usec
	);


	gmp = gmtime(&t);
	if (gmp == NULL) {
		errExit("gmtime");
	}


	// save a local copy of statically allocated memory
	gm = *gmp;

	printf("Broken down by gmtime():\n");
	printf("\tyear: %d\n", gm.tm_year);
	printf("\tmonth: %d\n", gm.tm_mon);
	printf("\tmonth-day: %d\n", gm.tm_mday);
	printf("\thour: %d\n", gm.tm_hour);
	printf("\tmin: %d\n", gm.tm_min);
	printf("\tsec: %d\n", gm.tm_sec);
	printf("\twday: %d\n", gm.tm_wday);
	printf("\tyday: %d\n", gm.tm_yday);
	printf("\tisdst: %d\n", gm.tm_isdst);


	locp = localtime(&t);
	if (locp == NULL) {
		errExit("localtime");
	}	

	// save local copy
	loc = *locp;


	printf("Broken down by localtime():\n");
	printf("\tyear: %d\n", loc.tm_year);
	printf("\tmonth: %d\n", loc.tm_mon);
	printf("\tmonth-day: %d\n", loc.tm_mday);
	printf("\thour: %d\n", loc.tm_hour);
	printf("\tmin: %d\n", loc.tm_min);
	printf("\tsec: %d\n", loc.tm_sec);
	printf("\twday: %d\n", loc.tm_wday);
	printf("\tyday: %d\n", loc.tm_yday);
	printf("\tisdst: %d\n", loc.tm_isdst);


	printf("\nasctime() formats gmtime() as: %s\n", asctime(&gm));
	printf("ctime() formats the time() value as: %s\n", ctime(&t));

	printf("mktime() of gmtime() value: %ld secs\n", (long)mktime(&gm));
	printf("mktime() of localtime() value: %ld secs\n", (long)mktime(&loc));

	exit(EXIT_SUCCESS);
}
