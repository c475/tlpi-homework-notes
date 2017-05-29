#include <utmp.h>
#include <utmpx.h>
#include <limits.h>
#include <paths.h>
#include "../../lib/tlpi_hdr.h"


// taking the question literally, implementing old style utmp functions
// since the manual page is old style


// incomplete, too weird. man pages are out of date and ambiguous.


void logwtmp2(const char *line, const char *name, const char *host)
{
    struct utmp ut;

    memset(ut, 0, sizeof(struct utmp));

    strncpy(ut.ut_line, line, __UT_LINESIZE - 1);
    strncpy(ut.ut_line, line, __UT_NAMESIZE - 1);
    strncpy(ut.ut_line, line, __UT_HOSTSIZE - 1);
    time((time_t *)&ut.ut_tv.tv_sec);
    ut.ut_pid = getpid();

    updwtmp(_PATH_WTMP, result);
}


void login2(const struct utmp *ut)
{
    char *tname;

    if (ut == NULL) {
        return;
    }

    tname = NULL;

    ut->ut_type = USER_PROCESS;
    ut->ut_pid = getpid();

    if (isatty(STDIN_FILENO)) {
        tname = ttyname(STDIN_FILENO);

    } else if (isatty(STDOUT_FILENO)) {
        tname = ttyname(STDOUT_FILENO);

    } else if (isatty(STDERR_FILENO)) {
        tname = ttyname(STDERR_FILENO);

    }

    if (tname != NULL && strcmp(tname, "/dev/") == 0) {
        tname = tname + 5;
    }

    if (tname != NULL) {
        strncpy(ut->ut_line, tname, __UT_LINESIZE - 1);
        pututline(ut);
        updwtmpx(_PATH_WTMP, &ut);
    } else {
        strncpy(ut->ut_line, "???", 3);
    }
}


void logout2(const char *ut_line)
{
    struct utmp search;
    struct utmp *result;

    memset(&search, 0, sizeof(struct ut));

    strncpy(search.ut_line, ut_line, __UT_LINESIZE - 1);

    setutent();

    result = getutline(&search);
    if (result == NULL) {
        return;
    }

    endutent();

    memset(result->ut_name, 0, sizeof(result->ut_name));
    memset(result->ut_host, 0, sizeof(result->ut_host));

    if (time((time_t *)&result->ut_tv.tv_sec) == -1) {
        return;
    }

    result->ut_type = DEAD_PROCESS;

    pututline(result);
    updwtmp(_PATH_WTMP, result);
}


int main(int argc, char *argv[])
{
    exit(EXIT_SUCCESS);
}
