#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "../../chapter-08/users_groups/ugid_functions.h"
#include "../../lib/tlpi_hdr.h"

#define MAX_PATH 1024
#define MAX_CONTENT 1024*4
#define MAX_USERNAME 1024


int checkUser(char *username, char *content)
{
	char *cptr = content;
	char *line = _strtok(cptr, '\n');
	int userMatch = 0;

	do {
		if (strncmp(line, "Uid", 3) == 0) {
			for (line = line + 4; isblank(*line); line++);

			if (strncmp(line, username, strlen(username)) == 0) {
				userMatch = 1;
			}
			break;
		}
	} while ((line = _strtok(NULL, '\n')) != NULL);

	return userMatch;
}


char *getCmdName(char *content)
{
	static char cmdName[1024];
	char *cptr = content;

	memset(cmdName, 0, 1024);

	char *line = _strtok(cptr, '\n');

	do {
		if (strncmp(line, "Name", 4) == 0) {
			for (line = line + 5; isblank(*line); line++);
			strncat(cmdName, line, 1024);
			break;
		}
	} while ((line = _strtok(NULL, '\n')) != NULL);

	return *cmdName ? cmdName : NULL;
}


char *strUid(char *username)
{
	static char uname[MAX_USERNAME];
	uid_t uid = userIdFromName(username);

	if (uid == -1) {
		return NULL;
	}

	snprintf(uname, MAX_USERNAME, "%ld", (long)uid);

	return uname;
}


// check if valid PID directory in /proc/ file system
int is_numeric_string(char *str)
{
	char *strp = str;
	int numeric = 1;

	if (strp == NULL || *strp == '\0') {
		return 0;
	}

	for (; *strp; strp++) {
		if (!(*strp >= '0' && *strp <= '9')) {
			numeric = 0;
			break;
		}
	}

	return numeric;
}


void printCmdLine(char *username, char *pid, char *content)
{
	char *cmd = getCmdName(content);

	if (cmd != NULL && checkUser(username, content)) {
		printf("PID: %s\nCOMMAND: %s\n\n", pid, cmd);
	}
}


int main(int argc, char *argv[])
{
	if (argc != 2) {
		usageErr("./program username\n");
	}

	int fd;
	char *uname;
	struct dirent *dp;
	DIR *dirp;

	char pathbuffer[MAX_PATH] = {0};
	char content[MAX_CONTENT] = {0};

	uname = strUid(argv[1]);
	if (uname == NULL) {
		errExit("could not find user %s\n", argv[1]);
	}

	dirp = opendir("/proc/");
	if (dirp == NULL) {
		errExit("opendir()");
	}

	while ((dp = readdir(dirp)) != NULL) {

		if (is_numeric_string(dp->d_name)) {

			snprintf(pathbuffer, MAX_PATH-1, "/proc/%s/status", dp->d_name);

			fd = open(pathbuffer, O_RDONLY);
			if (fd == -1) {
				printf("couldn't open %s\n", pathbuffer);
				continue;
			}

			if (read(fd, content, MAX_CONTENT-1) == -1) {
				errExit("read()");
			}

			close(fd);

			printCmdLine(uname, dp->d_name, content);

			memset(pathbuffer, 0, MAX_PATH);
			memset(content, 0, MAX_CONTENT);

		}
	}

	exit(EXIT_SUCCESS);
}
