#ifndef FILE_PERMS_HEADER
#define FILE_PERMS_HEADER

#include <sys/types.h>

// include set-user-id, set-group-id, and sticky bit info in returned string
#define FP_SPECIAL 1

char *filePermStr(mode_t perm, int flags);

#endif
