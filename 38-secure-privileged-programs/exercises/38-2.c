#define _DEFAULT_SOURCE // get getpass() declaration from <unistd.h>
#define _XPOEN_SOURCE 600 // get crypt() declaration from <unistd.h>

#include <crypt.h>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include "../../lib/tlpi_hdr.h"


static uid_t orig_euid = -1;


int regain_euid(void)
{
	if (orig_euid == -1) {
		orig_euid = geteuid();
	}

	return seteuid(orig_euid);
}


int drop_euid(void)
{
	if (orig_euid == -1) {
		orig_euid = geteuid();
	}

	return seteuid(getuid());
}


int main(int argc, char *argv[])
{
	char *username;
	char *password;
	char *encrypted;
	char *p;
	struct passwd *pwd;
	struct spwd *spwd;
	Boolean authOk;
	Boolean usernameProvided;

	if (drop_euid() == -1) { // DROP
		errExit("seteuid");
	}

	if (argc < 2) {
		usageErr("%s [ -u user ] program-file [args...]\n", argv[0]);
	}

	if (strcmp(argv[1], "-u") == 0) {
		if (argc < 4) {
			usageErr("%s [ -u user ] program-file [args...]\n", argv[0]);
		}

		username = argv[2];
		usernameProvided = TRUE;
	} else {
		username = malloc(5);
		strncpy(username, "root", 4);
		username[4] = '\0';
		usernameProvided = FALSE;
	}

	pwd = getpwnam(username);
	if (pwd == NULL) {
		fatal("Couldn't get password record");
	}

	if (regain_euid() == -1) { // REACQUIRE
		errExit("seteuid");
	}

	spwd = getspnam(username);
	if (spwd == NULL && errno == EACCES) {
		fatal("no permission to read shadow password file");
	}

	if (drop_euid() == -1) { // DROP
		errExit("seteuid");
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
		if (!usernameProvided) {
			free(username);
		}
		printf("Incorrect password\n");
		exit(EXIT_FAILURE);
	}

	// do authenticated work

	if (!usernameProvided) {
		free(username);
	}

	if (regain_euid() == -1) { // REACQUIRE
		errExit("seteuid");
	}

	// set UID, EUID, SUID to specified user (or root)
	if (setgid(pwd->pw_gid) == -1) {
		errExit("setgid");
	}

	// set GID, EGID, SGID to specified user (or root)
	if (setuid(pwd->pw_uid) == -1) {
		errExit("setuid");
	}

	if (usernameProvided) {
		execvp(argv[3], argv + 3);
	} else {
		execvp(argv[1], argv + 1);
	}

	printf("Failed to exec\n");

	exit(EXIT_FAILURE);
}
