#include <sys/types.h>
#include <sys/shm.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int i;
    int shmid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s shmid...\n", argv[0]);
    }

    for (i = 1; i < argc; i++) {
        shmid = getInt(argv[i], 0, "shmid");
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            errExit("shmctl");
        }
    }

    exit(EXIT_SUCCESS);
}
