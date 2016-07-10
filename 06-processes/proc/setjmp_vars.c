#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>


static jmp_buf env;


static void doJump(int nvar, int rvar, int vvar)
{
	printf("Inside doJump(): nvar=%d, rvar=%d, vvar:%d\n", nvar, rvar, vvar);
	longjmp(env, 1);
}


int main(int argc, char *argv[])
{
	int nvar;
	register int rvar; // allocate in register if possible
	volatile int vvar; // allocate in see text

	nvar = 111;
	rvar = 222;
	vvar = 333;

	// code executed after setjmp()
	if (setjmp(env) == 0) {
		nvar = 777;
		rvar = 888;
		vvar = 999;
		doJump(nvar, rvar, vvar);

	// coming from doJump
	} else {
		printf("After longjmp(): nvar=%d, rvar=%d, vvar=%d\n", nvar, rvar, vvar);
	}

	exit(EXIT_SUCCESS);
}
