#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>


mode_t getumask2(void)
{
	mode_t mask = umask(0);
	umask(mask);
	return mask;
}

int main(int argc, char *argv[])
{
	// not reentrant/thread-safe
	mode_t umask1 = getumask2();

	// gnu extension (_GNU_SOURCE), is reentrant/thread-safe
	mode_t umask2 = getumask();

	exit(EXIT_SUCCESS);
}
