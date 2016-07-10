#include <sys/xattr.h>
#include "../../lib/tlpi_hdr.h"

#define XATTR_SIZE 10000


static void usageError(char *progName)
{
	fprintf(stderr, "Usage: %s [-x] file\n", progName);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	char list[XATTR_SIZE];
	char value[XATTR_SIZE];
	ssize_t listLen;
	ssize_t valueLen;
	Boolean hexDisplay;
	
	int ns;
	int j;
	int k;
	int opt;

	hexDisplay = 0;
	while ((opt = getopt(argc, argv, "x")) != -1) {
		switch (opt) {
			case 'x':
				hexDisplay = 1;
				break;
			default:
				usageError(argv[0]);
		}
	}

	if (optind >= argc) {
		usageError(argv[0]);
	}

	for (j = optind; j < argc; j++) {
		listLen = listxattr(argv[j], list, XATTR_SIZE);
		if (listLen == -1) {
			errExit("listxattr");
		}

		printf("%s:\n", argv[j]);

		for (ns = 0; ns < listLen; ns += strlen(&list[ns]) + 1) {
			printf("        name=%s; ", &list[ns]);

			valueLen = getxattr(argv[j], &list[ns], value, XATTR_SIZE);
			if (valueLen == -1) {
				printf("couldnt get value");
			} else if (!hexDisplay) {
				printf("value=%.*s", (int)valueLen, value);
			} else {
				printf("value=");
				for (k = 0; k < valueLen; k++) {
					printf("%02x ", (unsigned int)value[k]);
				}
			}

			printf("\n");
		}

		printf("\n");
	}

	exit(EXIT_SUCCESS);
}
