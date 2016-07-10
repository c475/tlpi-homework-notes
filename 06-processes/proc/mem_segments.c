#include <stdio.h>
#include <stdlib.h>


chat globBuf[65536]; // uninitialized data segment
int primes[] = {2, 3, 5, 7}; // initialized data segment


static int square(int x) // allocated in frame square() (int x)
{
	int result;			// allocated in frame square
	result = x * x;
	return result;		// return value passed back via register
}


static void doCalc(int val) // allocated in frame doCalc() (int val)
{
	printf("The square of %d is %d\n", val, square(val));

	if (val < 1000) {
		int t;				// allocated in frame doCalc()

		t = val * val * val;
		printf("The cube of %d is %d\n", val, t);
	}
}


int main(int argc, char *argv[])
{
	// initialized in data segment. this is an interesting case.
	// this is how static variables in a function can retain their value
	// because they do not live in a stack frame
	// but in the initialized or uninitialized data segments
	static int key = 9973;

	static char mbuf[10240000]; // uninitialized data segment

	char *p;					// allocated for frame main()

	doCalc(key);

	exit(EXIT_SUCCESS);
}
