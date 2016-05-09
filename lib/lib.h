#ifndef _LIB_HEADER
#define _LIB_HEADER

#define _STRTOK_CONTENT_BUFFER_MAX 4084
#define _STRTOK_TOKENR_MAX 1024


struct LibWindow {
	int width;
	int height;
};


char *userNameFromId(uid_t uid);
uid_t userIdFromName(const char *name);
char *groupNameFromId(gid_t gid);
gid_t groupIdFromName(const char *name);
char *_strtok(char *content, char delim);
int isNumericString(char *str);
struct LibWindow getWindowSize(void);

#endif
