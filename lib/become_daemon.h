#ifndef _BECOME_DAEMON_HEADER

#define _BECOME_DAEMON_HEADER

#define BD_NO_CHDIR				1 // dont chdir("/")
#define BD_NO_CLOSE_FILES		2 // dont close all open files
#define BD_NO_REOPEN_STD_FDS 	4 // dont reopen stdin,stdout,stderr to /dev/null
#define BD_NO_UMASK0 			8 // dont do a umask(0)

#define BD_MAX_CLOSE 8192 // maximum file descriptors to close if sysconf(_SC_OPEN_MAX) is indeterminate

int becomeDaemon(int flags);

#endif
