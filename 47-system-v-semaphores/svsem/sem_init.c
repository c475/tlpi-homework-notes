// this is a snippet showing how to properly initialize a semaphore...without race conditions
// this is assuming multiple applications are all trying to access the semaphore

semid = semget(key, IPC_CREAT | IPC_EXCL | perms);

// successfully created the semaphore
if (semid != -1) {
    union semun arg;
    struct sembuf sop;

    arg.val = 0; // initialize to 0
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        errExit("semctl");
    }

    // perform a "no-op" semaphore operation
    // changes sem_otime so other processes can see we've initialized the set
    sop.sem_num = 0; // operate on semaphore 0
    sop.sem_op = 0;
    sop.sem_flg = 0;
    if (semop(semid, &sop, 1) == -1) {
        errExit("semop");
    }

// we did not create the semaphore
} else {
    const int MAX_TRIES = 10;
    int j;
    union semun arg;
    struct semid_ds ds;

    // unexpected error from semget()
    if (errno != EEXIST) {
        errExit("semget");
    }

    semid = semget(key, 1, perms); // retrieve id of the existing set
    if (semid == -1) {
        errExit("semid");
    }

    // wait until another proess has called semop()
    arg.buf = &ds;
    for (j = 0; j < MAX_TRIES; j++) {
        if (semctl(semid, 0, IPC_STAT, arg) == -1) {
            errExit("semctl");
        }

        // semop() performed?
        if (ds.sem_otime != 0) {
            break;
        }

        sleep(1); // if not, wait and retry
    }

    // loop ran to completion, something bad happened
    if (ds.sem_otime == 0) {
        fatal("existing semaphore not initialized");
    }
}


// now perform some operation on the semaphore...
