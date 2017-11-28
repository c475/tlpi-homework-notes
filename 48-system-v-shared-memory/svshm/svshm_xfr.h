#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "binary_sem.h"
#include "semun.h"
#include "../../lib/tlpi_hdr.h"

#define SHM_KEY 0x1234 // key for shated memory segment
#define SEM_KEY 0x5678 // key for semaphore set

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define WRITE_SEM 0 // writer has access to shared memory
#define READ_SEM 1 // reader has access to shared memory

#ifndef BUF_SIZE
#define BUF_SIZE 1024 // allow cc -D to override definition
#endif


struct shmseg {
    int cnt; // number of bytes used in "buf"
    char buf[BUF_SIZE]; // data being transferred
};
