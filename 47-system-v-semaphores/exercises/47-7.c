#include <sys/types.h>
#include <sys/sem.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"


#define MAX_SEMS 1024

struct sems {
    size_t len;
    struct sem_info *info;
};


struct sem_info {
    key_t key;
    int semid;
    mode_t perms;
    int nsems;
    uid_t uid;
    gid_t gid;
    uid_t cuid;
    gid_t cgid;
    time_t otime;
    time_t ctime;
};


struct sems *getSemaphores(void)
{
    int i;
    FILE *sem_file;
    char line[LINE_MAX];
    struct sems *s;
    struct sem_info *info;
    char *endptr;

    sem_file = fopen("/proc/sysvipc/sem", "r");
    if (sem_file == NULL) {
        return NULL;
    }

    info = calloc(MAX_SEMS, sizeof(struct sem_info));
    if (info == NULL) {
        return NULL;
    }

    // burn the first line
    if (fgets(line, LINE_MAX, sem_file) == NULL) {
        return NULL;
    }

    for (i = 0; fgets(line, LINE_MAX, sem_file) != NULL; i++) {
        // whatever
        info[i].key = strtol(line, &endptr, 10);
        info[i].semid = strtol(endptr, &endptr, 10);
        info[i].perms = strtol(endptr, &endptr, 8);
        info[i].nsems = strtol(endptr, &endptr, 8);
        info[i].uid = strtol(endptr, &endptr, 10);
        info[i].gid = strtol(endptr, &endptr, 10);
        info[i].cuid = strtol(endptr, &endptr, 10);
        info[i].cgid = strtol(endptr, &endptr, 10);
        info[i].otime = strtol(endptr, &endptr, 10);
        info[i].ctime = strtol(endptr, &endptr, 10);
    }

    fclose(sem_file);

    s = malloc(sizeof(struct sems));
    s->len = i;
    s->info = info;

    return s;
}


int main(int argc, char *argv[])
{
    int i;
    struct sems *s = getSemaphores();
    if (s == NULL) {
        errExit("getSemaphores");
    }

    printf("key\t\tsemid\t\towner\t\tperms\t\tnsems\n");

    for (i = 0; i < s->len; i++) {
        printf(
            "0x%08x\t%d\t\t%s\t\t%o\t\t%d\n",
            s->info[i].key,
            s->info[i].semid,
            userNameFromId(s->info[i].uid),
            s->info[i].perms,
            s->info[i].nsems
        );
    }

    free(s->info);
    free(s);

    exit(EXIT_SUCCESS);
}
