#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "../../lib/tlpi_hdr.h"
#include "semun.h"


int main(int argc, char *argv[])
{
    int semid;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s init-value\n   or: %s semid operation\n", argv[0], argv[0]);
    }

    // create and initialize semaphore
    if (argc == 2) {
        union semun arg;

        // create new semaphore set with 1 semaphore
        semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (semid == -1) {
            errExit("semid");
        }

        // initialize the semaphore: 0 is the index of the semaphore in the set
        arg.val = getInt(argv[1], 0, "init-value");
        if (semctl(semid, /* semnum= */ 0, SETVAL, arg) == -1) {
            errExit("semctl");
        }

        printf("Semaphore ID = %d\n", semid);

    // Perform an operation on first semaphore
    } else {
        struct sembuf sop; // structure defining operation on a semaphore...

        // id of the semaphore set
        semid = getInt(argv[1], 0, "semid");

        sop.sem_num = 0; // specified first semaphore in the set
        sop.sem_op = getInt(argv[2], 0, "operation"); // add, subtract, or wait for 0
        sop.sem_flg = 0; // no special options for operation

        printf("%ld: about to semop at %s\n", (long)getpid(), currTime("%T"));
        if (semop(semid, &sop, 1) == -1) {
            errExit("semop");
        }

        printf("%ld: semop completed at %s\n", (long)getpid(), currTime("%T"));
    }

    exit(EXIT_SUCCESS);
}
