// file status structure, some other things like chmod(), etc
#include <sys/stat.h>
// file control like O_CREAT, O_TRUNC, etc
#include <fcntl.h>

#include "../../lib/tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


int main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;

    // ssize_t lets a function return a negative error code
    // which is why it is being used over size_t here
    // because read() can return -1
    ssize_t numRead;

    char buf[BUF_SIZE] = {'\0'};

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s old-file new-file\n", argv[0]);
    }

    // open the input and output files

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        errExit("opening file %s", argv[1]);
    }

    /*
        O_CREAT: create file if it does not exist
        O_WRONLY: open file in read only mode
        O_TRUNC: if the file exists, truncate it
    */
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;

    /*
        rw-rw-rw

        read/write by anyone.

        Set these permission bits/flags in the event that the file must be created via O_CREAT
    */
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        errExit("opening file %s", argv[2]);
    }

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
            fatal("could not write whole buffer");
        }
    }

    if (numRead == -1) {
        errExit("read");
    }

    if (close(inputFd) == -1) {
        errExit("closing input file");
    }

    if (close(outputFd) == -1) {
        errExit("closing output file");
    }

    exit(EXIT_SUCCESS);
}
