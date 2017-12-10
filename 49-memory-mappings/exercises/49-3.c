#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


void handler(int sig)
{
    switch (sig) {
        case SIGBUS:
            write(STDOUT_FILENO, "SIGBUS\n", 7);
            break;

        case SIGSEGV:
            write(STDOUT_FILENO, "SIGSEGV\n", 8);
            break;

        default:
            write(STDOUT_FILENO, "WTF\n", 4);
            break;
    }
}


// I'm not getting it to work like in the book...
// Only way I get SIGBUS is to map a 0 length file and try to access the map
// the map is WAY bigger than it should be (in the book) when I create it too, so getting SIGSEGV is harder
int main(int argc, char *argv[])
{
    void *mem;
    void *sigbusAddr;
    void *sigsegvAddr;
    int fakeVal;
    long pageSize;
    int fileSize;
    int fd;
    struct sigaction sa;

    fakeVal = 0;
    fileSize = 1000;
    pageSize = sysconf(_SC_PAGE_SIZE);

    // signal handler for SIGBUS and SIGSEGV
    memset(&sa, 0, sizeof(struct sigaction));
    if (sigemptyset(&sa.sa_mask) == -1) {
        errExit("sigemptyset");
    }
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    if (sigaction(SIGBUS, &sa, NULL) == -1) {
        errExit("sigaction SIGBUS");
    }
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        errExit("sigaction SIGSEGV");
    }

    fd = open("/tmp/pagetest", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open");
    }

    // make file 500 bytes
    if (ftruncate(fd, 500) == -1) {
        errExit("ftruncate");
    }

    // map a 500 byte file to a 1000 byte mapping
    mem = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
        errExit("mmap");
    }

    // +300 from end of mapped file, yet still within the mapping
    sigbusAddr = mem + pageSize + 1000000;
    // 20 bytes outside of the effective mapping
    sigsegvAddr = mem + pageSize + 20;

    printf("Gonna SIGBUS\n");

    fakeVal = *(int *)sigbusAddr;

    printf("Gonna SIGSEGV\n");

    fakeVal = *(int *)sigsegvAddr;

    printf("All done I guess, here's fakeval for fun: %d\n", fakeVal);

    exit(EXIT_SUCCESS);
}
