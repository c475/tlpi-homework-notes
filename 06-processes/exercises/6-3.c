#include <errno.h>
#include "../../lib/tlpi_hdr.h"


/*
	Implement setenv() and unsetenv() using getenv(), putenv(), and, where necessary,
	code that directly modifies environ. Your version of unsetenv() should check to see
	whether there are multiple definitions of an environment variable, and remove
	them all (which is what the glibc version of unsetenv() does).

	This one is tricky. Especially avoiding memory leaks.
*/

extern char **environ;

int modenv_count = 0;


int _unsetenv(const char *name)
{

	if (!name || !*name) {
		errno = EINVAL;
		return -1;
	}

	char **envp = environ;
	char **envpp = envp;
	size_t name_len = strlen(name);

	while (*envp) {

		if (strncmp(name, *envp, name_len) == 0 && (*envp)[name_len] == '=') {

			for (envpp = envp; *envpp != NULL; envpp++) {
				*envpp = *(envpp+1);
			}

		} else {
			envp++;
		}
	}

	return 0;
}


int _setenv(const char *name, const char *val, int override)
{

	if (!name || !val || !*name || !*val) {
		errno = EINVAL;
		return -1;
	}

	char *new_env = NULL;

	size_t name_len = strlen(name);
	size_t val_len = strlen(val);

	if (getenv(name) && !override) {
		return 0;
	}

	if (unsetenv(name) == -1) {
		return -1;
	}


	new_env = malloc(name_len + 2 + val_len);

	strncpy(new_env, name, name_len);
	strncat(new_env, "=", 1);
	strncat(new_env, val, val_len);

	modenv[modenv_count++] = new_env;

	return (putenv(new_env) != 0) ? -1 :0;
}


int env_cleanup(void)
{
	while ((modenv_count-1)--) {
		free(modenv[modenv_count]);
	}

	return 0;
}


int main(int argc, char *argv[])
{
	clearenv();

    if (putenv("TT=xxxxx") != 0)
        perror("putenv");

    system("echo '***** Environment before unsetenv(TT)'; "
            "printenv | grep ^TT");
    system("echo 'Total lines from printenv:' `printenv | wc -l`");

    _unsetenv("TT");

    system("echo '***** Environment after unsetenv(TT)'; "
            "printenv | grep ^TT");
    system("echo 'Total lines from printenv:' `printenv | wc -l`");

    _setenv("xyz", "one", 1);
    _setenv("xyz", "two", 0);
    _setenv("xyz2", "222", 0);

    system("echo '***** Environment after setenv() calls'; "
            "printenv | grep ^x");
    system("echo 'Total lines from printenv:' `printenv | wc -l`");

    env_cleanup();

    exit(EXIT_SUCCESS);
}
