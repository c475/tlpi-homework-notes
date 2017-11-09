#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../lib/tlpi_hdr.h"

// #define OVERFLOW_SIZE 2770


#define STACK_SIZE 2048
#define OVERFLOW_SIZE 0
#define TEST_SIGNAL SIGUSR1


void handler(int sig)
{
	int i;
	// int overflow[OVERFLOW_SIZE] = {0}; // should overflow the provided stack?

	// srand(time(NULL));

	// // see if I can get something to happen by chance (possibly generating valid instructions)
	// for (i = 0; i < OVERFLOW_SIZE; i++) {
	// 	printf("%d\n", i);
	// 	overflow[i] = rand();
	// 	printf("%x\n", overflow[i]);
	// }

	// confirm that the handler finished executing (and presumably overwrote code in main())
	write(STDOUT_FILENO, "handler\n", 8);
}


// maybe try to overwrite the main program by giving the new signal stack the address of main,
// then providing the code of the new program in a buffer initialized in the signal handler?
int main(int argc, char *argv[])
{
	struct sigaction sa;
	char bad_stack_buffer[STACK_SIZE] = {0};
	stack_t bad_stack;


	bad_stack.ss_flags = 0;
	bad_stack.ss_sp = bad_stack_buffer;
	bad_stack.ss_size = STACK_SIZE;

	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_ONSTACK;
	sa.sa_handler = handler;

	if (sigaltstack(&bad_stack, NULL) == -1) {
		errExit("sigaltstack");
	}

	printf("Signal stack creation success\n");

	if (sigaction(TEST_SIGNAL, &sa, NULL) == -1) {
		errExit("sigaction");
	}

	printf("sigaction() success, going to raise %s\n", strsignal(TEST_SIGNAL));

	raise(TEST_SIGNAL);

	exit(EXIT_SUCCESS);
}
