#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int numSems;
    int semid;
    int flags;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s num-sems\n");
    }

    flags = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR;
    numSems = getInt(argv[1], 0, "numSems");
    semid = semget(IPC_PRIVATE, numSems, flags);

    if (semid == -1) {
        errExit("semid");
    }

    printf("id: %d\n", semid);

    exit(EXIT_SUCCESS);
}
