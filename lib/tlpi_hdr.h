#ifndef TLPI_HEADER

#define TLPI_HEADER

/*
    blkcnt_t
        Used for file block counts.
    blksize_t
        Used for block sizes.
    clock_t
        [XSI] [Option Start] Used for system times in clock ticks or CLOCKS_PER_SEC; see <time.h>. [Option End]
    clockid_t
        [TMR] [Option Start] Used for clock ID type in the clock and timer functions. [Option End]
    dev_t
        Used for device IDs.
    fsblkcnt_t
        [XSI] [Option Start] Used for file system block counts. [Option End]
    fsfilcnt_t
        [XSI] [Option Start] Used for file system file counts. [Option End]
    gid_t
        Used for group IDs.
    id_t
        [XSI] [Option Start] Used as a general identifier; can be used to contain at least a pid_t, uid_t, or gid_t. [Option End]
    ino_t
        Used for file serial numbers.
    key_t
        [XSI] [Option Start] Used for XSI interprocess communication. [Option End]
    mode_t
        Used for some file attributes.
    nlink_t
        Used for link counts.
    off_t
        Used for file sizes.
    pid_t
        Used for process IDs and process group IDs.
    pthread_attr_t
        [THR] [Option Start] Used to identify a thread attribute object. [Option End]
    pthread_barrier_t
        [BAR] [Option Start] Used to identify a barrier. [Option End]
    pthread_barrierattr_t
        [BAR] [Option Start] Used to define a barrier attributes object. [Option End]
    pthread_cond_t
        [THR] [Option Start] Used for condition variables. [Option End]
    pthread_condattr_t
        [THR] [Option Start] Used to identify a condition attribute object. [Option End]
    pthread_key_t
        [THR] [Option Start] Used for thread-specific data keys. [Option End]
    pthread_mutex_t
        [THR] [Option Start] Used for mutexes. [Option End]
    pthread_mutexattr_t
        [THR] [Option Start] Used to identify a mutex attribute object. [Option End]
    pthread_once_t
        [THR] [Option Start] Used for dynamic package initialization. [Option End]
    pthread_rwlock_t
        [THR] [Option Start] Used for read-write locks. [Option End]
    pthread_rwlockattr_t
        [THR] [Option Start] Used for read-write lock attributes. [Option End]
    pthread_spinlock_t
        [SPI] [Option Start] Used to identify a spin lock. [Option End]
    pthread_t
        [THR] [Option Start] Used to identify a thread. [Option End]
    size_t
        Used for sizes of objects.
    ssize_t
        Used for a count of bytes or an error indication.
    suseconds_t
        [XSI] [Option Start] Used for time in microseconds. [Option End]
    time_t
        Used for time in seconds.
    timer_t
        [TMR] [Option Start] Used for timer ID returned by timer_create(). [Option End]
    trace_attr_t
        [TRC] [Option Start] Used to identify a trace stream attributes object. [Option End]
    trace_event_id_t
        [TRC] [Option Start] Used to identify a trace event type. [Option End]
    trace_event_set_t
        [TRC TEF] [Option Start] Used to identify a trace event type set. [Option End]
    trace_id_t
        [TRC] [Option Start] Used to identify a trace stream. [Option End]
    uid_t
        Used for user IDs.
    useconds_t
        [XSI] [Option Start] Used for time in microseconds. [Option End]
*/

#include <sys/types.h>

#include <stdio.h>

/*
    (malloc, EXIT_SUCCESS, EXIT_FAILURE, etc)
*/
#include <stdlib.h>


/*
    int          access(const char *, int);
    unsigned int alarm(unsigned int);
    int          brk(void *);
    int          chdir(const char *);
    int          chroot(const char *); (LEGACY)
    int          chown(const char *, uid_t, gid_t);
    int          close(int);
    size_t       confstr(int, char *, size_t);
    char        *crypt(const char *, const char *);
    char        *ctermid(char *);
    char        *cuserid(char *s); (LEGACY)
    int          dup(int);
    int          dup2(int, int);
    void         encrypt(char[64], int);
    int          execl(const char *, const char *, ...);
    int          execle(const char *, const char *, ...);
    int          execlp(const char *, const char *, ...);
    int          execv(const char *, char *const []);
    int          execve(const char *, char *const [], char *const []);
    int          execvp(const char *, char *const []);
    void        _exit(int);
    int          fchown(int, uid_t, gid_t);
    int          fchdir(int);
    int          fdatasync(int);
    pid_t        fork(void);
    long int     fpathconf(int, int);
    int          fsync(int);
    int          ftruncate(int, off_t);
    char        *getcwd(char *, size_t);
    int          getdtablesize(void); (LEGACY)
    gid_t        getegid(void);
    uid_t        geteuid(void);
    gid_t        getgid(void);
    int          getgroups(int, gid_t []);
    long         gethostid(void);
    char        *getlogin(void);
    int          getlogin_r(char *, size_t);
    int          getopt(int, char * const [], const char *);
    int          getpagesize(void); (LEGACY)
    char        *getpass(const char *); (LEGACY)
    pid_t        getpgid(pid_t);
    pid_t        getpgrp(void);
    pid_t        getpid(void);
    pid_t        getppid(void);
    pid_t        getsid(pid_t);
    uid_t        getuid(void);
    char        *getwd(char *);
    int          isatty(int);
    int          lchown(const char *, uid_t, gid_t);
    int          link(const char *, const char *);
    int          lockf(int, int, off_t);
    off_t        lseek(int, off_t, int);
    int          nice(int);
    long int     pathconf(const char *, int);
    int          pause(void);
    int          pipe(int [2]);
    ssize_t      pread(int, void *, size_t, off_t);
    int          pthread_atfork(void (*)(void), void (*)(void), void(*)(void));
    ssize_t      pwrite(int, const void *, size_t, off_t);
    ssize_t      read(int, void *, size_t);
    int          readlink(const char *, char *, size_t);
    int          rmdir(const char *);
    void        *sbrk(intptr_t);
    int          setgid(gid_t);
    int          setpgid(pid_t, pid_t);
    pid_t        setpgrp(void);
    int          setregid(gid_t, gid_t);
    int          setreuid(uid_t, uid_t);
    pid_t        setsid(void);
    int          setuid(uid_t);
    unsigned int sleep(unsigned int);
    void         swab(const void *, void *, ssize_t);
    int          symlink(const char *, const char *);
    void         sync(void);
    long int     sysconf(int);
    pid_t        tcgetpgrp(int);
    int          tcsetpgrp(int, pid_t);
    int          truncate(const char *, off_t);
    char        *ttyname(int);
    int          ttyname_r(int, char *, size_t);
    useconds_t   ualarm(useconds_t, useconds_t);
    int          unlink(const char *);
    int          usleep(useconds_t);
    pid_t        vfork(void);
    ssize_t      write(int, const void *, size_t);
*/
#include <unistd.h>

#include <errno.h>

#include <string.h>

#include "get_num.h"

#include "error_functions.h"

#include "tlpi_time.h"

#include "lib.h"

#include "signal_functions.h"

#include "print_wait_status.h"

#include "strerror_nosafe.h"

#include "strerror_safe.h"

#include "print_rlimit.h"

#include "event_flags.h"

#include "semun.h"

#include "ds/list.h"

#include "ds/darray.h"

#include "ds/hashmap.h"

#include "wordlist.h"

#include "hashing/hash.h"

typedef enum { FALSE, TRUE } Boolean;


#define min(m, n) ((m) < (n) ? (m) : (n))
#define max(m, n) ((m) > (n) ? (m) : (n))

#endif
