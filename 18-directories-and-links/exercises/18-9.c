#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../../lib/tlpi_hdr.h"


/*
    I expect fchdir() to be more efficient because the system (or you) 
    don't have to work with a char buffer repeatedly. also:

    chdir() has to resolve the pathname part, maybe in a similar way to open(),
    then look up the inode in a file descriptor table, then when it gets the inode, 
    scans its contents for the directory in question.

    fchdir() skips the "resolving a pathname" step and jumps straight to the file descriptor table,
    that points to the inode entry containing a directory in it's data sector.

    something like that.
*/


/*
    RESULTS:

    $ mkdir test

    # chdir()
    $ time ./18-9 test -c 10000000

    real    0m4.373s
    user    0m0.292s
    sys     0m4.080s

    # fchdir()
    $ time ./18-9 test -f 10000000

    real    0m1.874s
    user    0m0.308s
    sys     0m1.564s


    fchdir() definitely wins.
    the extra time spent in user-space for fchdir may be the -1 check on both file descriptors?
*/

int main(int argc, char *argv[])
{
    int i;
    int is_fchdir;
    int fd1;
    int fd2;

    if (argc != 4) {
        usageErr("%s dir (-f or -c) num-iterations\n", argv[0]);
    }

    if (access(argv[1], F_OK) == -1) {
        errExit("access()");
    }

    is_fchdir = strcmp(argv[2], "-f") == 0;

    i = atoi(argv[3]);

    if (is_fchdir) {

        fd1 = open(".", O_RDONLY);
        fd2 = open(argv[1], O_RDONLY);

        if (fd1 == -1 || fd2 == -1) {
            if (fd1 != -1) {
                close(fd1);
            }

            if (fd2 != -1) {
                close(fd2);
            }

            errExit("open()");
        }

        while (i--) {

            if (fchdir(fd2) == -1) {
                errExit("fchdir()");
            }

            if (fchdir(fd1) == -1) {
                errExit("fchdir()");
            }
        }

    } else {

        while (i--) {

            if (chdir(argv[1]) == -1) {
                errExit("chdir()");
            }

            if (chdir("..") == -1) {
                errExit("chdir()");
            }
        }
    }

    if (is_fchdir) {
        close(fd1);
        close(fd2);
    }

    exit(EXIT_SUCCESS);
}
