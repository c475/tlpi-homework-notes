#include <unistd.h>
#include <stdio.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	char *term_path = ctermid(NULL);
	printf("pathname: %s\n", term_path);
	return 0;
}
