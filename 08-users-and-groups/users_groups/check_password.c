#define _BSD_SOURCE // get getpass() declaration from <unistd.h>
#define _XPOEN_SOURCE 600 // get crypt() declaration from <unistd.h>

#include <crypt.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	char *username, *password, *encrypted, *p;
	struct passwd *pwd;
	struct spwd *spwd;

	Boolean authOk;
	size_t len;
	long lnmax;

	printf("REAL UID: %ld\n", (long)getuid());
	printf("EFFE UID: %ld\n", (long)geteuid());

	lnmax = sysconf(_SC_LOGIN_NAME_MAX);

	// if limit cannot be determined
	if (lnmax == -1) {
		// make a guess...
		lnmax = 256;
	}

	username = malloc(lnmax);
	if (username == NULL) {
		errExit("malloc");
	}

	printf("Username ");
	fflush(stdout);
	if (fgets(username, lnmax, stdin) == NULL) {
		// exit on EOF
		exit(EXIT_FAILURE);
	}

	len = strlen(username);
	if (username[len-1] == '\n') {
		username[len-1] = '\0'; // remove the trailing slash and null terminate
	}

	pwd = getpwnam(username);
	if (pwd == NULL) {
		fatal("Couldn't get password record");
	}

	spwd = getspnam(username);
	if (spwd == NULL && errno == EACCES) {
		fatal("no permission to read shadow password file");
	}

	// there is a shadow password record, so use it
	if (spwd != NULL) {
		// overwrite pointer of passwd record with spwd record
		pwd->pw_passwd = spwd->sp_pwdp;
	}

	password = getpass("Password: ");

	encrypted = crypt(password, pwd->pw_passwd);

	// encrypt password and erase cleartext version immediately
	for (p = password; *p != '\0'; p++) {
		*p = '\0';
	}

	if (encrypted == NULL) {
		errExit("Crypt");
	}

	authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
	if (!authOk) {
		printf("Incorrect password\n");
		exit(EXIT_FAILURE);
	}

	printf("Successfully authenticated: UID=%ld\n", (long)pwd->pw_uid);

	// do authenticated work

	exit(EXIT_SUCCESS);
}
