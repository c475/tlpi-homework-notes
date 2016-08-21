#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "itimerspec_from_str.h"

/*
	Convert a string of the following form to an itimerspec structure:
	"value.sec[/value.nanosec][:interval.sec[/interval.nanosec]]".
	Optional components that are omitted cause 0 to be assigned to the
	corresponding structure fields.
*/

void itimerSpecFromStr(char *str, struct itimerspec *tsp)
{
	char *dupstr, *cptr, *sptr;

	dupstr = strdup(str);

	cptr = strchr(dupstr, ':');
	if (cptr != NULL) {
		*cptr = '\0';
	}

	sptr = strchr(dupstr, '/');
	if (sptr != NULL) {
		*sptr = '\0';
	}

	tsp->it_value.tv_sec = atoi(dupstr);
	tsp->it_value.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;

	if (cptr == NULL) {
		tsp->it_interval.tv_sec = 0;
		tsp->it_interval.tv_nsec = 0;
	} else {
		sptr = strchr(cptr + 1, '/');
		if (sptr != NULL) {
			*sptr = '\0';
		}

		tsp->it_interval.tv_sec = atoi(cptr + 1);
		tsp->it_interval.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
	}

	free(dupstr);
}
