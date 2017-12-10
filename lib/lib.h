#ifndef _LIB_HEADER
#define _LIB_HEADER

#define _STRTOK_CONTENT_BUFFER_MAX 4084
#define _STRTOK_TOKENR_MAX 1024

#include <sys/stat.h>


struct WindowDimensions {
	int width;
	int height;
};

typedef struct Status {
    pid_t pid;
    char comm[128];
    char state;
    pid_t ppid;
    gid_t pgrp;
    struct Status *prev;
    struct Status *next;
    int flags;
} Status;


char *userNameFromId(uid_t uid);
uid_t userIdFromName(const char *name);
char *groupNameFromId(gid_t gid);
gid_t groupIdFromName(const char *name);
char *_strtok(char *content, char delim);
int isNumericString(char *str);
struct WindowDimensions getWindowSize(void);
Status *getProcessStatus(pid_t pid);
int endswith(char *buffer, char *suffix);
int startswith(char *buffer, char *prefix);
void displayStatInfo(const struct stat *sb);

#endif
