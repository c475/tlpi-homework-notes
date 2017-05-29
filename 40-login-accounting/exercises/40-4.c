#define _GNU_SOURCE
#include <utmpx.h>
#include <time.h>
#include <paths.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    struct utmpx *record;

    // utmpxname(_PATH_WTMP);

    while ((record = getutxent()) != NULL) {

        if (record->ut_type == USER_PROCESS) {

            time_t ok = record->ut_tv.tv_sec;

            printf("%-8s %-8s (%s)\n",
                record->ut_user,
                ctime(&ok),
                record->ut_line
            );
        }
    }

    exit(EXIT_SUCCESS);
}
