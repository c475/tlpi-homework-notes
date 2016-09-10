#define _BSD_SOURCE
#include <stdlib.h>
#include "../../lib/tlpi_hdr.h"


struct person {
	char *name;
	int age;
	int weight;
};


static void atexitFunc1(void)
{
	printf("atexit function 1 called\n");
}


static void atexitFunc2(void)
{
	printf("atexit function 2 called\n");
}


static void onexitFunc(int exitStatus, void *arg)
{
	struct person *p = (struct person *)arg;
	printf("on_exit function called: status=%d, arg=%s\n", exitStatus, p->name);
}


int main(int argc, char *argv[])
{
	struct person p;

	p.name = calloc(1, 128);
	strncpy(p.name, "Some Guy", 128);
	p.age = 26;
	p.weight = 200;

	if (on_exit(onexitFunc, (void *)&p) != 0) {
		fatal("on_exit 1");
	}

	if (atexit(atexitFunc1) != 0) {
		fatal("atexit 1");
	}

	if (atexit(atexitFunc2) != 0) {
		fatal("atexit 2");
	}

	if (on_exit(onexitFunc, (void *)&p) != 0) {
		fatal("on_exit 2");
	}

	exit(2);
}
