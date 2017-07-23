#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/msg.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	int maxind; // max index of message queue entries (kernel data structure)
	int i;
	int msqid;
	struct msqid_ds ds;
	struct msginfo msginfo;

	// obtain side of kernel "entries" array
	maxind = msgctl(0, MSG_INFO, (struct msqid_ds *)&msginfo);
	if (maxind == -1) {
		errExit("msgctl");
	}

	printf("maxind: %d\n\n", maxind);
	printf("index    id        key       messages\n");

	// retrueve and display mq info
	for (i = 0; i < maxind; i++) {
		// remember with the MSG_STAT operator, it expects an IJNDEX to the entries array
		msqid = msgctl(i, MSG_STAT, &ds);
		if (msqid == -1) {
			if (errno != EINVAL && errno != EACCES) {
				errMsg("msgctl-MSG_STAT"); // unexpected error
			}
			continue; // ignore this item
		}

		printf("%4d %8d  0x%08lx %7ld\n", i, msqid, (unsigned long)ds.msg_perm.__key, (long)ds.msg_qnum);
	}

	exit(EXIT_SUCCESS);
}
