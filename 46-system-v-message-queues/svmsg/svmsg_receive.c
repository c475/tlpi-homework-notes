#define _GNU_SOURCE // get definition of MSG_EXCEPT
#include <sys/types.h> // compat
#include <sys/msg.h>
#include "../../lib/tlpi_hdr.h"

#define MAX_MTEXT 1024


// the message
struct mbuf {
	long mtype; // message type
	char mtext[MAX_MTEXT]; // message body
};


static void usageError(const char *progName, const char *msg)
{
	if (msg != NULL) {
		fprintf(stderr, "%s", msg);
	}

	fprintf(stderr, "Usage: %s [options] msqid [max-bytes]\n", progName);
    fprintf(stderr, "Permitted options are:\n");
    fprintf(stderr, "    -e        Use MSG_NOERROR flag\n");
    fprintf(stderr, "    -t type   Select message of given type\n");
    fprintf(stderr, "    -n        Use IPC_NOWAIT flag\n");

#ifdef MSG_EXCEPT
    fprintf(stderr, "    -x        Use MSG_EXCEPT flag\n"); // linux only
#endif
}


int main(int argc, char *argv[])
{
    int msqid;
    int flags;
    int type;
    ssize_t msgLen;
    size_t maxBytes;
    struct mbuf msg; // message buffer for msgrcv()
    int opt; // command line argument flags

    // parse command-line options and args

    flags = 0;
    type = 0;

    while ((opt = getopt(argc, argv, "ent:x")) != -1) {

        switch (opt) {
            case 'e':
                flags |= MSG_NOERROR;
                break;

            case 'n':
                flags |= IPC_NOWAIT;
                break;

            #ifdef MSG_EXCEPT
            case 'x':
                flags |= MSG_EXCEPT;
                break;
            #endif

            case 't':
                type = atoi(optarg);
                break;

            default:
                usageError(argv[0], NULL);
        }
    }

    if (argc < optind + 1 || argc > optind + 2) {
        usageError(argv[0], "Wrong number of arguments\n");
    }

    msqid = getInt(argv[optind], 0, "msqid");
    maxBytes = argc > optind + 1 ? getInt(argv[optind + 1], 0, "max-bytes") : MAX_MTEXT;

    // get a message and display on stdout

    msgLen = msgrcv(msqid, &msg, maxBytes, type, flags);
    if (msgLen == -1) {
        errExit("msgrcv");
    }

    printf("Received: type=%ld; length=%ld", msg.mtype, (long)msgLen);
    if (msgLen > 0) {
        printf("; body=%s", msg.mtext);
    }

    printf("\n");

    exit(EXIT_SUCCESS);
}
