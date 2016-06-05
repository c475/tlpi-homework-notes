#include <sys/stat.h>
#include <stdio.h>
#include "file_perms.h"

/* Interface for this implementation */
#define STR_SIZE sizeof("rwxrwxrwx")

/* Return ls(1)-style string for file permissions mask */
char *filePermStr(mode_t perm, int flags)
{
	static char str[STR_SIZE];
	snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
		(perm & S_IRUSR) ? 'r' : '-',
		(perm & S_IWUSR) ? 'w' : '-',
		(perm & S_IXUSR) ?
			(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
			(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
		(perm & S_IRGRP) ? 'r' : '-',
		(perm & S_IWGRP) ? 'w' : '-',
		(perm & S_IXGRP) ?
			(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
			(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
		(perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
		(perm & S_IXOTH) ?
			(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
			(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-')
	);

	return str;
}
