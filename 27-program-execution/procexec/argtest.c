#include <stdio.h>
#include <string.h>


void printargs(char *name, ...)
{
	size_t last_len = strlen(name);

	for (;;) {
		printf("%s\n", name);
		name -= last_len + 1;
		if (name - sizeof(char *) == NULL) {
			break;
		}
		last_len = strlen(name);
	}
}


int main(int argc, char *argv[])
{
	printargs("TEST", "arg1", "arg2", "arg3", "arg4", (char *)NULL);

	return 0;
}
