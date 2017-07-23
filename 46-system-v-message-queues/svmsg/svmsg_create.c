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


int main(int argc, char *argv[])
{
    int numKeyFlags; // counts -f, -k, and -p options
    int flags; // flags to use in msgget()
    int msqid; // id of the message queue
    int opt;
    unsigned int perms;
    long lkey;
    key_t key;

    // parse command line options and arguments

    numKeyFlags = 0;
    flags = 0;

    // this can be confusing if you try to pass "0" as the key and consistently get a different message queue ID.
    // that is because on this system, IPC_PRIVATE == 0
    while ((opt = getopt(argc, argv, "cf:k:px")) != -1) {

        switch (opt) {

            case 'c':
                printf("IPC_CREAT\n");
                flags |= IPC_CREAT;
                break;

            case 'f': // -f pathname
                key = ftok(optarg, 1);
                if (key == -1) {
                    errExit("ftok");
                }
                numKeyFlags++;
                break;

            case 'k':
                if (sscanf(optarg, "%li", &lkey) != 1) {
                    cmdLineErr("-k option required a numeric argument");
                }
                key = lkey;
                numKeyFlags++;
                break;

            case 'p':
                key = IPC_PRIVATE;
                numKeyFlags++;
                break;

            case 'x':
                flags |= IPC_EXCL;
                break;

            default:
                printf("%d\n", opt);
                usageError(argv[0], "Bad option\n");
        }
    }

    if (numKeyFlags != 1) {
        usageError(argv[0], "Exactly one of the options -f, -k, or -p must be supplied\n");
    }

    perms = optind == argc ? (S_IRUSR | S_IWUSR) : getInt(argv[optind], GN_BASE_8, "octal-perms");

    printf("KEY: %ld\n", (long)key);

    msqid = msgget(key, flags | perms);
    if (msqid == -1) {
        errExit("msgget");
    }

    printf("%d\n", msqid);

    exit(EXIT_SUCCESS);
}
