#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include "../../lib/tlpi_hdr.h"
#include "ugid_functions.h"

char *userNameFromId(uid_t uid)
{
	struct passwd *pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}

uid_t userIdFromName(const char *name)
{
	struct passwd *pwd;

	// cant pass in null pointer or empty string
	if (name == NULL || *name == '\0') {
		return -1;
	}

	pwd = getpwnam(name);
	if (pwd == NULL) {
		return -1;
	}

	return pwd->pw_uid;
}

char *groupNameFromId(gid_t gid)
{
	struct group *grp = getgrgid(gid);
	return grp == NULL ? NULL : grp->gr_name;
}

gid_t groupIdFromName(const char *name)
{
	struct group *grp;

	if (name == NULL || *name == '\0') {
		return -1;
	}

	grp = getgrnam(name);
	if (grp == NULL) {
		return -1;
	}

	return grp->gr_gid;
}
