#define _GNU_SOURCE
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    struct utmpx *ut;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("%s [utmp-pathname]\n", argv[0]);
    }

    if (argc > 1) {
        // use alternate pathname
        if (utmpxname(argv[1]) == -1) {
            errExit("utmpxname");
        }
    }

    // rewind to beginning of utmp file just in case
    setutxent();

    printf("user     type        PID line   id  host      date/time\n");

    // sequential scan until EOF
    while ((ut = getutxent()) != NULL) {
        printf("%-8s ", ut->ut_user);
        printf("%-9.9s ",
            ut->ut_type == EMPTY ?          "EMPTY" :
            ut->ut_type == RUN_LVL ?        "RUN_LVL" :
            ut->ut_type == BOOT_TIME ?      "BOOT_TIME" :
            ut->ut_type == NEW_TIME ?       "NEW_TIME" :
            ut->ut_type == OLD_TIME ?       "OLD_TIME" :
            ut->ut_type == INIT_PROCESS ?   "INIT_PR" :
            ut->ut_type == LOGIN_PROCESS ?  "LOGIN_PR" :
            ut->ut_type == USER_PROCESS ?   "USER_PR" :
            ut->ut_type == DEAD_PROCESS ?   "DEAD_PR" : "???"
        );

        printf("%5ld %-6.6s %-3.5s %-9.9s ", (long)ut->ut_pid, ut->ut_line, ut->ut_id, ut->ut_host);
        printf("%s", ctime((time_t *)&(ut->ut_tv.tv_sec)));
    }

    endutxent();

    exit(EXIT_SUCCESS);
}
