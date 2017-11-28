#include <sys/types.h>
#include <sys/sem.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


union semun { // used in calls to semctl()
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};



int main(int argc, char *argv[])
{
    pid_t childPid;
    union semun arg;
    struct sembuf sops;
    int semId;

    semId = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);
    if (semId == -1) {
        errExit("semget()");
    }

    // initialize semaphore to "in use"
    arg.val = 0;
    if (semctl(semId, 0, SETVAL, arg) == -1) {
        errExit("semctl()");
    }

    switch (childPid = fork()) {

        case -1:
            errExit("fork");

        case 0:
            // child does some required action here
            printf("[%s %ld] Child started - doing work\n", currTime("%T"), (long)getpid());
            sleep(2);

            printf("[%s %ld] Child about to release the semaphore\n", currTime("%T"), (long)getpid());

            // release the semaphore and unblock parent
            sops.sem_num = 0;
            sops.sem_op = 1;
            sops.sem_flg = 0;
            if (semop(semId, &sops, 1) == -1) {
                errExit("semop() child");
            }

            // now child can do other things

            _exit(EXIT_SUCCESS);

        default:
            // parent can do some work here, 
            // then waits for child to complete required action

            printf("[%s %ld] Parent about to subtract 1 from semaphore\n", currTime("%T"), (long)getpid());
            sops.sem_num = 0;
            sops.sem_op = -1;
            sops.sem_flg = 0;
            if (semop(semId, &sops, 1) == -1) {
                errExit("semop() child");
            }

            printf("[%s %ld] Parent subtracted 1 from semaphore\n", currTime("%T"), (long)getpid());
    }

    // delete the semaphore
    if (semctl(semId, 0, IPC_RMID, arg) == -1) {
        errExit("semctl() IPC_RMID");
    }

    exit(EXIT_SUCCESS);
}
