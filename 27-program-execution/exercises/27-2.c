#include <limits.h>
#include <stdarg.h>
#include "../../lib/tlpi_hdr.h"

#define MAX_ARGS 10
#define MAX_ARG_LEN 1024

extern char **environ;


int execlp_t(const char *file, ...)
{
	int i;
	char *fcopy;
	char *next;
	char *argVec[MAX_ARGS];
	char *path;
	char *pathSegment;
	char *pathCopy;
	char *a;
	char fullPath[PATH_MAX];
	size_t segmentLen;
	size_t fileLen;

	va_list argList;
	va_start(argList, file);

	// get rid of valgrind uninitialized memory errors/warnings
	for (i = 0; i < MAX_ARGS; i++) {
		argVec[i] = NULL;
	}

	fileLen = strlen(file);

	// const qualifier... have to make a copy
	fcopy = calloc(fileLen + 1, 1);
	strncpy(fcopy, file, fileLen);

	argVec[0] = fcopy;

	for (i = 1; i < MAX_ARGS - 1; i++) {
		a = va_arg(argList, char *);

		if (a == NULL) {
			break;
		}

		// getconf ARG_MAX is too big, be more reasonable (1KB)
		next = calloc(MAX_ARG_LEN, 1);

		strncpy(next, a, MAX_ARG_LEN - 1);

		argVec[i] = next;
	}

	// either it is in the current directory with chmod +x
	// or need to search $PATH
	if (strchr(file, '/') == NULL) {

		execve(file, argVec, environ);

		// exec failed, try $PATH
		path = getenv("PATH");
		if (path == NULL) {
			return -1;
		}

		// have to make a copy of $PATH so strtok doesn't mangle it (static buffer)
		// bc the environ is passed to the new program
		pathCopy = calloc(strlen(path) + 1, 1);
		strncpy(pathCopy, path, strlen(path));

		pathSegment = strtok(pathCopy, ":");

		// unfortunate that /bin/ is so close to the end of $PATH
		do {
			segmentLen = strlen(pathSegment);

			// pathname is too big, don't even try
			if (segmentLen + fileLen >= PATH_MAX - 1) {
				continue;
			}

			memset(fullPath, 0, PATH_MAX);
			strncpy(fullPath, pathSegment, segmentLen);

			while (*pathSegment++ != '\0');

			if (*pathSegment - 1 != '/') {
				strncat(fullPath, "/", 1);
			}

			strncat(fullPath, file, fileLen);

			execve(fullPath, argVec, environ);

		} while ((pathSegment = strtok(NULL, ":")) != NULL);

		// couldn't find executable in $PATH either, clean up and return -1
		for (i = 0; i < MAX_ARGS - 1 && argVec[i] != NULL; i++) {
			free(argVec[i]);
		}

		free(pathCopy);

		return -1;
	}

	// explicit path. just try to exec it
	execve(file, argVec, environ);

	// nope
	for (i = 0; i < MAX_ARGS - 1 && argVec[i] != NULL; i++) {
		free(argVec[i]);
	}

	return -1;
}


int main(int argc, char *argv[])
{
	execlp_t("./ls", "-l", (char *)NULL);
	exit(EXIT_SUCCESS);
}
