#define _GNU_SOURCE // MAP_ANONYMOUS
#include <sys/mman.h>
#include "../../lib/tlpi_hdr.h"

#define LEN (1024*1024)

#define SHELL_FMT "cat /proc/%ld/maps | grep zero"
#define CMD_SIZE (sizeof(SHELL_FMT) + 20)


int main(int argc, char *argv[])
{
    char cmd[CMD_SIZE];
    char *addr;

    // create an anonymous mapping with all access denied
    addr = mmap(NULL, LEN, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        errExit("mmap");
    }

    // display line in /proc/self/maps corresponding to the anonymous mapping
    printf("before mprotect()\n");
    snprintf(cmd, CMD_SIZE, SHELL_FMT, (long)getpid());
    system(cmd);

    // change proterction on memory to allow read and wrtie access
    if (mprotect(addr, LEN, PROT_READ | PROT_WRITE) == -1) {
        errExit("mprotect");
    }

    printf("after mprotect()\n");
    system(cmd); // review the protection in the maps file

    exit(EXIT_SUCCESS);
}
