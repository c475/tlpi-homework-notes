#include <time.h>
#include "tlpi_time.h"

#define BUF_SIZE 1024

/* 
	Return a string containing the current time formatted according to
	the specification in 'format' (see strftime(3) for specifiers).
	If 'format' is NULL, we use "%c" as a specifier (which gives the
	date and time as for ctime(3), but without the trailing newline).
	Returns NULL on error.
*/

char *currTime(const char *format)
{
	// nonreentrant
	static char buf[BUF_SIZE];

	time_t t;
	size_t s;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL) {
		return NULL;
	}

	s = strftime(buf, BUF_SIZE, format != NULL ? format : "%c", tm);

	return (s == 0) ? NULL : buf;
}
