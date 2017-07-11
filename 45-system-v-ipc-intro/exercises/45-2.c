#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


key_t ftok2(const char *pathname, int proj)
{
    key_t ipc_key;
    struct stat statbuf;

    ipc_key = 0;

    if (lstat(pathname, &statbuf) == -1) {
        return -1;
    }

    ipc_key |= statbuf.st_ino & 0xFFFF;
    ipc_key |= (minor(statbuf.st_dev) & 0xFF) << 16;
    ipc_key |= (proj & 0xFF) << 24;

    return ipc_key;
}


int main(int argc, char *argv[])
{
    int fd;
    int proj;
    key_t ipc_key;
    key_t ipc_key_ftok2;

    if (argc != 3) {
        usageErr("%s keyfile proj\n", argv[0]);
    }

    fd = open(argv[1], O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open");
    }

    close(fd);

    proj = getInt(argv[2], GN_NONNEG, 0);

    ipc_key = ftok(argv[1], proj);
    if (ipc_key == -1) {
        errExit("ftok");
    }

    ipc_key_ftok2 = ftok2(argv[1], proj);
    if (ipc_key_ftok2 == -1) {
        errExit("ftok2");
    }

    printf("ftok():            %x\n", ipc_key);
    printf("ftok2():           %x\n", ipc_key_ftok2);
    printf("ftok() == ftok2(): %s\n", ipc_key == ipc_key_ftok2 ? "true" : "false");

    exit(EXIT_SUCCESS);
}
