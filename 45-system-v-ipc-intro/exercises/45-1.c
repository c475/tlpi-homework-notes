#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define KEY_FILE "keyfile"
#define PROJ_VAL 'a'


int main(int argc, char *argv[])
{
    int fd;
    key_t ipc_key;
    struct stat statbuf;

    fd = open(KEY_FILE, O_CREAT | O_RDONLY, S_IWUSR | S_IRUSR);
    if (fd == -1) {
        errExit("open(): couldn't create IPC key file");
    }

    if (fstat(fd, &statbuf) == -1) {
        errExit("fstat(): couldn't retrieve info about IPC key file");
    }

    ipc_key = ftok(KEY_FILE, PROJ_VAL);
    if (ipc_key == -1) {
        errExit("ftok");
    }

    printf("key file inode:    %x\n", (int)(statbuf.st_ino & 0xFFFF));
    printf("minor dev num:     %x\n", minor(statbuf.st_dev));
    printf("proj value:        %x\n", PROJ_VAL);
    printf("resulting ipc_key: %x\n", ipc_key);

    close(fd);

    exit(EXIT_SUCCESS);
}
