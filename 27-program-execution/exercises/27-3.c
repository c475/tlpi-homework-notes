// my guess is that "Hello world" is printed twice
// 1  Hello world
// 2  Hello world

#include "../../lib/tlpi_hdr.h"


extern char **environ;

// oh dang, it prints the contents out...
int main(int argc, char *argv[])
{
	execve("./myscript.sh", argv, environ);
}
