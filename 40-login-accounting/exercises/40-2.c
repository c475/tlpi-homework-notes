#define _GNU_SOURCE
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int fd;
    struct utmpx ut;
    struct lastlog llog;
    char *devName;
    uid_t uid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s username [sleep-time]\n", argv[0]);
    }

    // initialize login record for utmp and wtmp files

    memset(&ut, 0, sizeof(struct utmpx));
    ut.ut_type = USER_PROCESS; // this is a user login
    strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));

    if (time((time_t *)&ut.ut_tv.tv_sec) == -1) {
        errExit("time");
    }

    ut.ut_pid = getpid();

    /* Set ut_line and ut_id based on the terminal associated with
    'stdin'. This code assumes terminals named "/dev/[pt]t[sy]*".
    The "/dev/" dirname is 5 characters; the "[pt]t[sy]" filename
    prefix is 3 characters (making 8 characters in all). */

    devName = ttyname(STDIN_FILENO);
    if (devName == NULL) {
        errExit("ttyname");
    }

    // should never happen
    if (strlen(devName) <= 8) {
        fatal("Terminal name is too short: %s", devName);
    }

    strncpy(ut.ut_line, devName + 5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, devName + 8, sizeof(ut.ut_id));

    printf("Creating login entries in utmp and wtmp\n");
    printf("      using pid %ld, line %.*s, id %.*s\n",
        (long)ut.ut_pid, (int)sizeof(ut.ut_line), 
        ut.ut_line, (int)sizeof(ut.ut_id), ut.ut_id);

    setutxent(); // rewind to start of utmp file
    if (pututxline(&ut) == NULL) { // write login record to utmp
        errExit("pututxline");
    }

    updwtmpx(_PATH_WTMP, &ut); // append login record to wtmp

    // sleep for awhile to examime the utmp and wtmp files
    sleep((argc > 2) ? getInt(argv[2], GN_NONNEG, "sleep-time") : 15);

    // now do a "logout"; use values from previously initialized "ut"
    ut.ut_type = DEAD_PROCESS; // required for logout record
    time((time_t *)&ut.ut_tv.tv_sec); // stamp with logout time
    memset(ut.ut_user, 0, sizeof(ut.ut_user));

    printf("Creating logout entries in utmp and wtmp\n");
    setutxent(); // rewind to start of utmp file

    if (pututxline(&ut) == NULL) { // overwrite previous utmp record with logout
        errExit("pututxline");
    }

    updwtmpx(_PATH_WTMP, &ut); // append logout to wtmp

    endutxent();

    // write to lastlog
    uid = userIdFromName(argv[1]);
    if (uid == -1) {
        errExit("userIdFromName");
    }

    fd = open(_PATH_LASTLOG, O_WRONLY);
    if (fd == -1) {
        errExit("open");
    }

    if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1) {
        errExit("lseek");
    }

    if (write(fd, &llog, sizeof(struct lastlog)) == -1) {
        errExit("write");
    }

    close(fd);

    exit(EXIT_SUCCESS);
}
