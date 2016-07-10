#include <pwd.h>
#include "../../lib/tlpi_hdr.h"


struct passwd *_getpwnam(const char *name)
{
	struct passwd *pwd = NULL;

	while ((pwd = getpwent()) != NULL) {

		if (strcmp(pwd->pw_name, name) == 0) {
			break;
		}
	}

	endpwent();

	return pwd;
}


int main(int argc, char *argv[])
{
	if (argc != 2) {
		return -1;
	}

	struct passwd *pwd = _getpwnam(argv[1]);

	if (pwd == NULL) {
		exit(EXIT_FAILURE);
	}

	printf("FOUND... UID: %ld\n", (long)pwd->pw_uid);

	pwd = _getpwnam(argv[1]);

	if (pwd == NULL) {
		printf("YEP... NULL\n");
	} else {
		printf("FOUND... UID: %ld\n", (long)pwd->pw_uid);
	}


	exit(EXIT_SUCCESS);
}
