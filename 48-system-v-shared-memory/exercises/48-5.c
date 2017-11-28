#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"


#define MAX_ITEMS 100
#define MAX_NAME 1024
#define MAX_VALUE 1024
#define IPC_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)


struct item {
    int active;
    char name[MAX_NAME];
    char value[MAX_VALUE];
};

// just go the array route, dealing with address offsets in shared memory is a pain
// it scales poorly but whatever
struct index {
    int semid;
    struct item items[MAX_ITEMS];
};


// wait for zero + increment semaphore atomically, dont care about signals
int wait_turn(int semid)
{
    struct sembuf sops[2];

    sops[0].sem_num = 0;
    sops[0].sem_op = 0;
    sops[0].sem_flg = 0;
    sops[1].sem_num = 0;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;

    return semop(semid, sops, 2);
}


// pretty bad if this fails
int release_sem(int semid)
{
    struct sembuf sops[1];

    sops[0].sem_num = 0;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    return semop(semid, sops, 1);
}


char *search_item(struct index *idx, const char *name)
{
    int i;
    char *value;

    if (wait_turn(idx->semid) == -1) {
        return NULL;
    }

    value = NULL;
    for (i = 0; i < MAX_ITEMS; i++) {
        if (idx->items[i].active && strcmp(idx->items[i].name, name) == 0) {
            value = idx->items[i].value;
            break;
        }
    }

    if (release_sem(idx->semid) == -1) {
        printf("that sucks\n");
    }

    return value;
}


int set_item(struct index *idx, const char *name, const char *value)
{
    int i;
    int found;

    if (wait_turn(idx->semid) == -1) {
        return -1;
    }

    found = 0;
    for (i = 0; i < MAX_ITEMS; i++) {
        if (idx->items[i].active && strcmp(idx->items[i].name, name) == 0) {
            strncpy(idx->items[i].value, value, MAX_VALUE);
            found = 1;
            break;
        }
    }

    if (!found) {
        // find first free block
        for (i = 0; i < MAX_ITEMS; i++) {
            if (idx->items[i].active == 0) {
                break;
            }
        }

        // we're full
        if (i >= MAX_ITEMS) {
            return -1;
        }

        idx->items[i].active = 1;
        strncpy(idx->items[i].name, name, MAX_NAME);
        strncpy(idx->items[i].value, value, MAX_VALUE);
    }

    if (release_sem(idx->semid) == -1) {
        printf("that sucks\n");
    }

    return 0;
}


int delete_item(struct index *idx, const char *name)
{
    int i;

    if (wait_turn(idx->semid) == -1) {
        return -1;
    }

    for (i = 0; i < MAX_ITEMS; i++) {
        if (idx->items[i].active && strcmp(idx->items[i].name, name) == 0) {
            // just mark it inactive
            idx->items[i].active = 0;
            return 0;
        }
    }

    if (release_sem(idx->semid) == -1) {
        printf("that sucks\n");
    }

    return -1;
}

// create or retrieve the shared memory
struct index *ini_keystore(key_t key)
{
    int semid;
    int shmid;
    struct index *idx;
    int created;
    union semun dummy;

    shmid = shmget(key, sizeof(struct index), 0);
    // check if exists or not
    if (shmid == -1) {
        if (errno == ENOENT) {
            shmid = shmget(key, sizeof(struct index), IPC_CREAT | IPC_EXCL | IPC_PERMS);
            if (shmid == -1) {
                return NULL;
            }

            created = 1;

        } else {
            return NULL;
        }

    } else {
        created = 0;
    }

    idx = shmat(shmid, NULL, 0);
    // should maybe delete the shared memory too, idk
    if (idx == (void *)-1) {
        return NULL;
    }

    // if it's a new shared memory segment, have to create a semahore for it
    if (created) {
        semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | IPC_PERMS);
        if (semid == -1) {
            return NULL;
        }

        memset(idx, 0, sizeof(struct index));

        idx->semid = semid;

    } else {
        // make sure the semaphore still exists
        if (semctl(idx->semid, 1, IPC_STAT, &dummy) == -1) {
            // make a new semaphore if the old one is gone
            semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | IPC_PERMS);
            if (semid == -1) {
                return NULL;
            }

            idx->semid = semid;
        }
    }

    return idx;
}


// test driver
int main(int argc, char *argv[])
{
    struct index *idx;
    key_t key;

    if (argc < 3) {
        usageErr("%s action key [value]\n  actions are [get|set|delete]\n", argv[0]);
    }

    key = 0x1234;

    idx = ini_keystore(key);
    if (idx == NULL) {
        errExit("ini_keystore");
    }

    if (strcmp(argv[1], "get") == 0) {
        char *value = search_item(idx, argv[2]);
        if (value != NULL) {
            printf("%s\n", value);
        } else {
            printf("%s not found\n", argv[2]);
        }

    } else if (strcmp(argv[1], "set") == 0) {
        if (set_item(idx, argv[2], argv[3]) == -1) {
            errExit("set_item");
        }

    } else if (strcmp(argv[1], "delete") == 0) {
        if (delete_item(idx, argv[2]) == -1) {
            errExit("delete_item");
        }

    } else {
        usageErr("invalid action %s\n", argv[1]);
    }

    if (shmdt(idx) == -1) {
        errExit("shmdt");
    }

    exit(EXIT_SUCCESS);
}
