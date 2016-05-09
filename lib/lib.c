#include "tlpi_hdr.h"
#include "lib.h"
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/ioctl.h>


char *_strtok(char *content, char delim)
{
	static char _strtok_content_buffer[_STRTOK_CONTENT_BUFFER_MAX];
	static char _strtok_token_buffer[_STRTOK_TOKENR_MAX];

	static char *_strtok_token = NULL;
	static char *_strtok_tbp;

	if (content != NULL) {
		_strtok_token = NULL;
		_strtok_tbp = NULL;
		memset(_strtok_content_buffer, 0, _STRTOK_CONTENT_BUFFER_MAX);
		memcpy(_strtok_content_buffer, content, _STRTOK_CONTENT_BUFFER_MAX);
	}

	if (_strtok_token == NULL) {
		_strtok_token = _strtok_content_buffer;
		_strtok_tbp = _strtok_token;
	}

	while (*_strtok_tbp && *_strtok_tbp != delim) _strtok_tbp++;

	if (*_strtok_tbp == '\0') {
		return NULL;
	}

	memset(_strtok_token_buffer, 0, _STRTOK_TOKENR_MAX);

	*_strtok_tbp = '\0';
	snprintf(_strtok_token_buffer, _STRTOK_TOKENR_MAX-1, "%s", _strtok_token);
	*_strtok_tbp++ = delim;
	_strtok_token = _strtok_tbp;

	return _strtok_token_buffer;
}


int isNumericString(char *str)
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


char *userNameFromId(uid_t uid)
{
	struct passwd *pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}


uid_t userIdFromName(const char *name)
{
	struct passwd *pwd;

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


struct LibWindow getWindowSize(void)
{
	struct winsize ws;
	struct LibWindow win;
	ioctl(0, TIOCGWINSZ, &ws);
	win.width = ws.ws_col;
	win.height = ws.ws_row;
	return win;
}
