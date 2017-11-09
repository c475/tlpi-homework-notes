#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void exit_handler1(void)
{
	printf("In exit handler 1\n");
	exit(EXIT_SUCCESS);
}


void exit_handler2(void)
{
	printf("In exit handler 2\n");
	exit(EXIT_SUCCESS);
}


// looks like it doesn't recursively call the exit handlers
int main(int argc, char *argv[])
{
	atexit(exit_handler1);
	atexit(exit_handler2);
	exit(EXIT_SUCCESS);
}
