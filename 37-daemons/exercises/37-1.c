#include <syslog.h>
#include "../../lib/tlpi_hdr.h"


int getLevel(char *level)
{
	int ret;

	if (strcmp(level, "debug") == 0) {
		ret = LOG_DEBUG;
	} else if (strcmp(level, "info") == 0) {
		ret = LOG_INFO;
	} else if (strcmp(level, "notice") == 0) {
		ret = LOG_NOTICE;
	} else if (strcmp(level, "warning") == 0) {
		ret = LOG_WARNING;
	} else if (strcmp(level, "error") == 0) {
		ret = LOG_ERR;
	} else if (strcmp(level, "critical") == 0) {
		ret = LOG_CRIT;
	} else if (strcmp(level, "alert") == 0) {
		ret = LOG_ALERT;
	} else if (strcmp(level, "emergency") == 0) {
		ret = LOG_EMERG;
	} else {
		ret = LOG_DEBUG;
	}

	return ret;
}


int main(int argc, char *argv[])
{
	int level;

	if (argc < 2) {
		usageErr("%s message [level]\n", argv[0]);
	}

	if (argc > 2) {
		level = getLevel(argv[2]);
	} else {
		level = LOG_DEBUG;
	}

	syslog(level, "%s", argv[1]);

	exit(EXIT_SUCCESS);
}
