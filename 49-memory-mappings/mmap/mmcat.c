#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    char *addr;
    int fd;
    struct stat sb;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s file\n", argv[0]);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        errExit("open");
    }

    // get file size and use it in mmap call
    if (fstat(fd, &sb) == -1) {
        errExit("fstat");
    }

    // this is helpful when handing stranger files/devices to mmap()
    printf("\n\n");
    displayStatInfo(&sb);
    printf("\n\n");

    addr = mmap(NULL, sb.st_blksize * 25, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }

    if (write(STDOUT_FILENO, addr, sb.st_size) != sb.st_size) {
        fatal("partial/failed write");
    }

    exit(EXIT_SUCCESS);
}
