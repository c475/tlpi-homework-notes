#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	int i;
	printf("priotiry: %d\n", getpriority(PRIO_PROCESS, 0));
	for (i = 0; i < argc; i++) {
		printf("argv[%d]: %s\n", i, argv[i]);
	}
	return 0;
}
