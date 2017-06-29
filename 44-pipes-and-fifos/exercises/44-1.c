#include <limits.h>
#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int i;
    size_t len;
    char line[PIPE_BUF];
    int fds1[2]; // parent read fds1[0], child write fds1[1]
    int fds2[2]; // child read fds2[0], parent write fds2[1]

    memset(line, 0, PIPE_BUF);

    if (pipe(fds1) == -1) {
        errExit("pipe 1");
    }

    if (pipe(fds2) == -1) {
        errExit("pipe 2");
    }

    switch (fork()) {

        case -1:
            errExit("fork");

        case 0:
            if (close(fds1[0]) == -1) {
                errExit("close 1");
            }

            if (close(fds2[1]) == -1) {
                errExit("close 2");
            }

            // loop until EOF
            while (read(fds2[0], line, PIPE_BUF) != 0) {
                len = strlen(line);

                for (i = 0; i < len; i++) {
                    if (line[i] >= 'a' && line[i] <= 'z') {
                        line[i] -= 32;
                    }
                }

                if (write(fds1[1], line, len + 1) != len + 1) {
                    errExit("write (child)");
                }
            }

            if (close(fds1[1]) == -1) {
                errExit("close 7");
            }

            if (close(fds2[0]) == -1) {
                errExit("close 8");
            }

            _exit(EXIT_SUCCESS);

        default:
            if (close(fds1[1]) == -1) {
                errExit("close 3");
            }

            if (close(fds2[0]) == -1) {
                errExit("close 4");
            }

            while (fgets(line, PIPE_BUF, stdin) != NULL) {
                len = strlen(line) + 1;

                if (write(fds2[1], line, len) != len) {
                    errExit("write (parent)");
                }

                if (read(fds1[0], line, len) != len) {
                    errExit("read (parent)");
                }

                printf("%s", line);
            }

            if (close(fds1[0]) == -1) {
                errExit("close 5");
            }

            if (close(fds2[1]) == -1) {
                errExit("close 6");
            }

            break;
    }

    wait(NULL);

    exit(EXIT_SUCCESS);
}
