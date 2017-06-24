#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int pfd[2]; // pipe file descriptors

    // create the pipe
    if (pipe(pfd) == -1) {
        errExit("pipe");
    }

    switch (fork()) {

        case -1:
            errExit("fork");

        // first child: exec "ls" to write to pipe
        case 0:
            if (close(pfd[0]) == -1) {
                errExit("close");
            }

            // duplicate stdout on write end of pipe; close duplicated descriptor
            // defensive check
            if (pfd[1] != STDOUT_FILENO) {
                if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                    errExit("dup2 1");
                }

                if (close(pfd[1]) == -1) {
                    errExit("close 2");
                }
            }

            execlp("ls", "ls", (char *)NULL);
            errExit("exexlp ls");

        // parent falls through to create next child
        default:
            break;
    }

    switch (fork()) {

        case -1:
            errExit("fork");

        // second child: exec 'wc' to read from pipe
        case 0:
            if (close(pfd[1]) == -1) {
                errExit("close 3");
            }

            // duplicate 
            if (pfd[0] != STDOUT_FILENO) {
                if (dup2(pfd[0], STDIN_FILENO) == -1) {
                    errExit("dup2 2");
                }

                if (close(pfd[0]) == -1) {
                    errExit("close 4");
                }
            }

            execlp("wc", "wc", "-l", (char *)NULL);
            errExit("execlp wc");

        // parent falls through again
        default:
            break;
    }

    if (close(pfd[0]) == -1) {
        errExit("close 5");
    }

    if (close(pfd[1]) == -1) {
        errExit("close 6");
    }

    if (wait(NULL) == -1) {
        errExit("wait 1");
    }

    if (wait(NULL) == -1) {
        errExit("wait 2");
    }

    exit(EXIT_SUCCESS);
}
