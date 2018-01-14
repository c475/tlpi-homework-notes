#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


// really not as clean as i was expecting
// mmap() is one of those things that is best handled in wrapper functions at least
int main(int argc, char *argv[])
{
    int srcfd;
    int destfd;
    struct stat sb;
    // really have to do this?
    char *src_contents;
    char *dest_contents;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s src dest\n");
    }

    srcfd = open(argv[1], O_RDONLY);
    if (srcfd == -1) {
        errExit("open");
    }

    // sb.st_size
    if (fstat(srcfd, &sb) == -1) {
        errExit("fstat");
    }

    src_contents = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0);
    if (src_contents == MAP_FAILED) {
        errExit("mmap");
    }

    if (close(srcfd) == -1) {
        errExit("close");
    }

    destfd = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP);
    if (destfd == -1) {
        errExit("open");
    }

    // have to make sure the destination file is as large as the source file
    if (ftruncate(destfd, sb.st_size) == -1) {
        errExit("ftruncate");
    }

    dest_contents = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, destfd, 0);
    if (dest_contents == MAP_FAILED) {
        errExit("mmap");
    }

    memcpy(dest_contents, src_contents, sb.st_size);

    // make sure the memory map is synced to disk
    if (msync(dest_contents, sb.st_size, 0) == 1) {
        errExit("msync");
    }

    if (close(destfd) == -1) {
        errExit("close");
    }

    if (munmap(dest_contents, sb.st_size) == -1) {
        errExit("munmap");
    }

    if (munmap(src_contents, sb.st_size) == -1) {
        errExit("munmap");
    }

    exit(EXIT_SUCCESS);
}
