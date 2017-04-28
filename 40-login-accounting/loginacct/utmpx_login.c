#define _GNU_SOURCE
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    struct utmpx *ut;
    char *devName;

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
}