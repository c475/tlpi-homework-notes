#include <ctype.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"

#define POPEN_FMT "/bin/ls -d %s 2> /dev/null"
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)


int main(int argc, char *argv[])
{
    char pat[PAT_SIZE]; // pattern for globbing
    char popenCmd[PCMD_BUF_SIZE];
    FILE *fp; // file stream returned by popen()
    Boolean badPattern; // invalid characters in 'pat'?
    int len;
    int status;
    int fileCnt;
    int j;
    char pathname[PATH_MAX];

    for (;;) {
        printf("pattern: ");
        fflush(stdout);
        if (fgets(pat, PAT_SIZE, stdin) == NULL) {
            break;
        }

        len = strlen(pat);

        // empty line
        if (len <= 1) {
            continue;
        }

        // strip trailing newline
        if (pat[len - 1] == '\n') {
            pat[len - 1] = '\0';
        }

        /* Ensure that the pattern contains only valid characters,
        i.e., letters, digits, underscore, dot, and the shell
        globbing characters. (Our definition of valid is more
        restrictive than the shell, which permits other characters
        to be included in a filename if they are quoted.) */

        for (j = 0, badPattern = FALSE; j < len && !badPattern; j++) {
            if (!isalnum((unsigned char)pat[j]) && strchr("_*?[^-].", pat[j]) == NULL) {
                badPattern = TRUE;
            }
        }

        if (badPattern) {
            printf("Bad pattern character: %c\n", pat[j - 1]);
            continue;
        }

        // build and execute command to glob "pat"

        snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat);

        fp = popen(popenCmd, "r");
        if (fp == NULL) {
            printf("popen() failed\n");
            continue;
        }

        // read resulting list of parameters until EOF

        fileCnt = 0;
        while (fgets(pathname, PATH_MAX, fp) != NULL) {
            printf("%s", pathname);
            fileCnt++;
        }

        // close pipe, fetch and display termination status

        status = pclose(fp);
        printf("    %d matching file%s\n", fileCnt, (fileCnt != 1) ? "s" : "");
        printf("    pclose() status = %#x\n", (unsigned int)status);
        if (status != -1) {
            printWaitStatus("\t", status);
        }
    }

    exit(EXIT_SUCCESS);
}
