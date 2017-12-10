#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    void *mem;
    int fd;

    fd = open("/etc/passwd", O_RDONLY);

    mem = mmap(NULL, 1024, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mem == MAP_FAILED) {
        printf("failed\n");
    }

    return 0;
}
