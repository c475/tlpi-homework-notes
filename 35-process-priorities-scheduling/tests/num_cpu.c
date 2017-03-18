#include <unistd.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	int numCPU;

	numCPU = sysconf(_SC_NPROCESSORS_ONLN);

	printf("num cpu: %d\n", numCPU);

	return 0;
}