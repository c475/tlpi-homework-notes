#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define FIFO_NAME "some_test_fifo"
#define BUF_SIZE 1024


// basically try to read and write to a non-blocking FIFO as fast as possible,
// hopefully demonstrating the consequences of O_NONBLOCK on both a read and write FIFO.
// after testing it's pretty race-conditiony but you can eventually trigger all the outcomes
int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    size_t num;
    int read_fifo;
    int flags;
    int fd;

    if (argc != 2) {
        usageErr("%s mode", argv[0]);
    }

    if (mkfifo(FIFO_NAME, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        errExit("mkfifo");
    }

    // You want the parent to be the read FIFO, or the write FIFO?
    read_fifo = strcmp(argv[1], "r") == 0;

    switch (fork()) {

        case -1:
            errExit("fork");

        case 0:
            // set up corresponding fifo
            if (read_fifo) {
                flags = O_WRONLY | O_NONBLOCK;
                while ((fd = open(FIFO_NAME, flags)) == -1 && errno == ENXIO) {
                    printf("[CHILD] Parent hasn't open()ed FIFO for reading yet\n");
                }

                if (fd == -1) {
                    errExit("open (child)");
                }

                for (;;) {
                    num = write(fd, "HELLO", 7);
                    if (num == 0) {
                        printf("[CHILD] Parent wasn't read()ing\n");
                    }
                }

            } else {
                flags = O_RDONLY | O_NONBLOCK;
                fd = open(FIFO_NAME, flags);
                if (fd == -1) {
                    errExit("open (child)");
                }

                for (;;) {
                    num = read(fd, buf, BUF_SIZE);
                    if (num == 0) {
                        printf("[CHILD] Parent hasn't done write()\n");
                    }
                }
            }

            _exit(EXIT_FAILURE);

            break;

        default:
            if (read_fifo) {
                flags = O_RDONLY | O_NONBLOCK;
                fd = open(FIFO_NAME, flags);
                if (fd == -1) {
                    errExit("open (child)");
                }

                for (;;) {
                    num = read(fd, buf, BUF_SIZE);
                    if (num == 0) {
                        printf("[PARENT] Child hasn't done write()\n");
                    }
                }

            } else {
                flags = O_WRONLY | O_NONBLOCK;
                while ((fd = open(FIFO_NAME, flags)) == -1 && errno == ENXIO) {
                    printf("[PARENT] Child hasn't open()ed FIFO for reading yet\n");
                }

                if (fd == -1) {
                    errExit("open (child)");
                }

                for (;;) {
                    num = write(fd, "HELLO", 7);
                    if (num == 0) {
                        printf("[PARENT] Child wasn't read()ing\n");
                    }
                }
            }

            exit(EXIT_FAILURE);
            break;
    }

    exit(EXIT_SUCCESS);
}
