#define _BSD_SOURCE // getpass()
#define _XOPEN_SOURCE // crypt()

#include <sys/capability.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include "../../lib/tlpi_hdr.h"


// change setting of capability in caller's effective capabilities
static int modifyCap(int capability, int setting)
{
	cap_t caps;
	cap_value_t capList[1];

	// retrieve caller's current capabilities
	caps = cap_get_proc();
	if (caps == NULL) {
		return -1;
	}

	// change setting of 'capability' in the effective set of 'caps'
	// The third argument, 1, is the number of items in the array capList
	capList[0] = capability;
	if (cap_set_flag(caps, CAP_EFFECTIVE, 1, capList, setting) == -1) {
		cap_free(caps);
		return -1;
	}

	// push modified capability sets back to kernel, to change caller's capabilities
	if (cap_set_proc(caps) == -1) {
		cap_free(caps);
		return -1;
	}

	// free the structure that was allocated by libcap
	if (cap_free(caps) == -1) {
		return -1;
	}

	return 0;
}


static int raiseCap(int capability)
{
	return modifyCap(capability, CAP_SET);
}


// an analogous dropCap() (unneeded in this program) could be
// defined as: modifyCap(capability, CAP_CLEAR)
// initializes an empty set of capabilities and sends it to kernel
static int dropAllCaps(void)
{
	cap_t empty;
	int s;

	empty = cap_init();
	if (empty == NULL) {
		return -1;
	}

	s = cap_set_proc(empty);
	if (cap_free(empty) == -1) {
		return -1;
	}

	return s;
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
	size_t len;
	long lnmax;

	lnmax = sysconf(_SC_LOGIN_NAME_MAX);
	if (lnmax == -1) {
		// if the limit it indeterminate, take a guess
		lnmax = 256;
	}

	username = malloc(lnmax);
	if (username == NULL) {
		errExit("malloc");
	}

	printf("Username: ");
	fflush(stdout);
	if (fgets(usename, lmax, stdin) == NULL) {
		free(username);
		exit(EXIT_FAILURE);
	}

	len = strlen(username);
	if (username[len - 1] == '\n') {
		username[len - 1] = '\0'; // remove trailing newline
	}

	pwd = getpwnam(username);
	if (pwd == NULL) {
		fatal("Couln't get password record");
	}

	// only raise CAP_DAC_READ_SEARCH for as long as we need it
	if (raiseCap(CAP_DAC_READ_SEARCH == -1) {
		fatal("raiseCap() failed");
	}

	spwd = getspnam(username);
	if (spwd == NULL && errno == EACCESS) {
		fatal("No permission to read shadow password file");
	}

	// at this point, we won't need any more capabilities
	// so drop them all
	if (dropAllCaps() == -1) {
		fatal("dropAllCaps() failed");
	}

	if (spwd != NULL) { // If there is a shadow password record
		pwd->pw_passwd = spwd->sp_pwdp; // use the shadow password
	}

	password = getpass("Password: ");

	encrypted = crypt(password, pwd->pw_passwd);
	for (p = passwd; *p != '\0';) { // clear plaintext password immediately
		*p++ = '\0';
	}

	if (encrypted == NULL) {
		errExit("crypt");
	}

	authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
	if (!authOk) {
		printf("Incorrect password\n");
		exit(EXIT_FAILURE);
	}

	printf("Successfully authenticated: UID=%ld\n", (long)pwd->pw_uid);

	// now do authenticated work...

	exit(EXIT_SUCCESS);
}
