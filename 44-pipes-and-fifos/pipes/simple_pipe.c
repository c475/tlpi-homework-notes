#include <sys/wait.h>
#include "../../lib/tlpi_hdr.h"


#define BUF_SIZE 10


int main(int argc, char *argv[])
{
    int pfd[2];
    char buf[BUF_SIZE];
    ssize_t numRead;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s string\n", argv[0]);
    }

    if (pipe(pfd) == -1) {
        errExit("pipe");
    }

    switch (fork()) {

        case -1:
            errExit("fork");

        // child reads
        case 0:
            // write end is unused
            if (close(pfd[1]) == -1) {
                errExit("close - child");
            }

            // read data on pipe, echo it to stdout
            for (;;) {
                numRead = read(pfd[0], buf, BUF_SIZE);
                if (numRead == -1) {
                    errExit("read");
                }

                // end of file, write end is closed
                if (numRead == 0) {
                    break;
                }

                if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                    fatal("child - partial/failed write");
                }
            }

            write(STDOUT_FILENO, "\n", 1);
            if (close(fd[0]) == -1) {
                errExit("close");
            }

            _exit(EXIT_SUCCESS);

        // parent writes
        default:
            if (close(pfd[0]) == -1) {
                errExit("close - parent");
            }

            if (write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
                fatal("parent - partial/failed write");
            }

            // child will see EOF
            if (close(pfd[1]) == -1) {
                errExit("close");
            }

            // wait for child to finish
            wait(NULL);
            break;
    }

    exit(EXIT_SUCCESS);
}
