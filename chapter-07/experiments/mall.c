#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	int *lol = malloc(1024);

	printf("SIZE?: %d\n", *(lol-2));

	return 0;
}