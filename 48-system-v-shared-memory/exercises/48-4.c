#include <sys/shm.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int shmid;
    struct shmid_ds ds;
    char abuf[26];
    char dbuf[26];
    char cbuf[26];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s shmid\n", argv[0]);
    }

    shmid = getInt(argv[1], 0, "shmid");

    if (shmctl(shmid, IPC_STAT, &ds) == -1) {
        errExit("shmctl");
    }

    printf("========== %d ==========\n", shmid);
    printf("ipc_perm\n");
    printf("  key: 0x%08x\n", ds.shm_perm.__key);
    printf("  user: %s\n", userNameFromId(ds.shm_perm.uid));
    printf("  group: %s\n", groupNameFromId(ds.shm_perm.gid));
    printf("  cuser: %s\n", userNameFromId(ds.shm_perm.cuid));
    printf("  cgroup: %s\n", groupNameFromId(ds.shm_perm.cgid));
    printf("  mode: %o\n", ds.shm_perm.mode);
    printf("  seq: %o\n", ds.shm_perm.__seq);
    printf("shm_segsz: %ld\n", (long)ds.shm_segsz);

    strncpy(abuf, ctime(&ds.shm_atime), 26);
    strncpy(dbuf, ctime(&ds.shm_dtime), 26);
    strncpy(cbuf, ctime(&ds.shm_ctime), 26);
    abuf[24] = '\0';
    dbuf[24] = '\0';
    cbuf[24] = '\0';

    printf("shm_atime: %s\n", abuf);
    printf("shm_dtime:%s\n", dbuf);
    printf("shm_ctime: %s\n", cbuf);
    printf("shm_cpid: %ld\n", (long)ds.shm_cpid);
    printf("shm_lpid: %ld\n", (long)ds.shm_lpid);
    printf("shm_nattch: %ld\n\n", (long)ds.shm_nattch);

    exit(EXIT_SUCCESS);
}
