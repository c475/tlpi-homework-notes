#include <setjmp.h>
#include "../../lib/tlpi_hdr.h"


/*
	Nothing happened... if you compile without optimisation
	WITH optimisation, the function loops forever until the stack overflows into the heap.
	That's pretty strange behavior.
*/


static jmp_buf env;


static int lets_longjmp(void)
{
	if (setjmp(env) == 0) {

		printf("RIGHT AFTER SETJMP\n");

		return 1;
	}

	printf("LONG JUMP\n");

	return 2;
}


int main(int argc, char *argv[])
{
	lets_longjmp();

	longjmp(env, 1);

	exit(EXIT_SUCCESS);
}
