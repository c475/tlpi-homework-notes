#include <utmpx.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"


char *getlogin2(void)
{
    struct utmpx ut_search;
    struct utmpx *ut_result;
    char *tname;

    ut_result = NULL;

    memset(&ut_search, 0, sizeof(struct utmpx));

    tname = ttyname(STDIN_FILENO);
    if (tname == NULL) {
        return NULL;
    }

    ut_search.ut_type = USER_PROCESS;
    strncpy(ut_search.ut_line, tname, __UT_LINESIZE - 1);

    setutxent();

    ut_result = getutxline(&ut_search);
    if (ut_result == NULL) {
        return NULL;
    }

    endutxent();

    return ut_result->ut_user;
}


int main(int argc, char *argv[])
{
    printf("getlogin: %s\n", getlogin());
    printf("TTYNAME: %s\n", getlogin2());
    exit(EXIT_SUCCESS);
}
