/*
	No output is produced because printf is line buffered when talking to a terminal
	When exec() happens, the stdio buffer no longer exists
*/
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	printf("Hello world");
	execlp("sleep", "sleep", "0", (char *) NULL);
}
