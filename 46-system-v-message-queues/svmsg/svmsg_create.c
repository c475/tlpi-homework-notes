#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "../../lib/tlpi_hdr.h"


static void usageError(const char *progName, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s", msg);
    }

    fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} [octal-perms]\n", progName);
    fprintf(stderr, "    -c            Use IPC_CREAT flag\n");
    fprintf(stderr, "    -x            Use IPC_EXCL flag\n");
    fprintf(stderr, "    -f pathname   Generate key using ftok()\n");
    fprintf(stderr, "    -k key        Use 'key' as key\n");
    fprintf(stderr, "    -p            Use IPC_PRIVATE key\n");

    exit(EXIT_FAILURE);
}

