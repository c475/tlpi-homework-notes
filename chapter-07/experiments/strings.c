#include "../../lib/tlpi_hdr.h"


typedef struct MemoryNode {

	int start;
	int end;
	int size;

} MemoryNode;


typedef struct ProgramBreak {

	int start;
	int end;
	MemoryNode *nodes;

} ProgramBreak;



/*
	Test out all of the string.h functions as a refresher
*/

size_t _strcspn(const char *s1, const char *s2)
{
	const char *s1p = s1;
	const char *s2p = s2;

	int match = 0;
	size_t num = 0;

	for (s1p = s1; *s1p != '\0'; s1p++, num++) {

		for (s2p = s2; *s2p != '\0'; s2p++) {

			if (*s1p == *s2p) {
				match = 1;
				break;
			}
		}

		if (match) {
			break;
		}
	}

	return num;
}


int main(int argc, char *argv[])
{
	char *literal = "ThisIs=LiteralString";
	char *ok = (char *)memchr(literal, '=', strlen(literal));


	printf("=== memchr(\"ThisIs=LiteralString\", '=', strlen(literal)) ===\n");
	printf("literal: %s\n", literal); // ThisIs=LiteralString
	printf("memchr result: %s\n", ok); // =LiteralString


	printf("=== memcmp('cool', 'beans', 4); ===\n");
	printf("%d\n", memcmp("cool", "beans", 4)); // 1
	printf("=== memcmp('coop', 'cool', 4); ===\n");
	printf("%d\n", memcmp("coop", "cool", 4)); // 1
	printf("=== memcmp('beans', 'beans', 4); ===\n");
	printf("%d\n", memcmp("beans", "beans", 4)); // 0
	printf("=== memcmp('aaa', 'bbb', 4); ===\n");
	printf("%d\n", memcmp("aaa", "bbb", 4)); // -1


	char b1[5] = {0};
	char b2[5] = {0};

	memcpy(b1, "cool", 4);
	memcpy(b2, "bean", 4);
	memcpy(b1+1, "ean", 3);

	printf("memcpy res: %s\n", b1); // cean

	/*
		strcat(src, dst) append dst to src
		strncat - same thing except specify a number to avoid buffer overrun
	*/


	printf("strcspn: %ld\n", (long)strcspn("cool", "tesc")); // 3?
	printf("_strcspn: %ld\n", (long)_strcspn("cool", "tesc")); // 3?

	exit(EXIT_SUCCESS);
}


