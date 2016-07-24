#include <signal.h>


void _abort(void)
{
	raise(SIGABRT);
	signal(SIGABRT, SIG_DFL);
	raise(SIGABRT);
}


int main(int argc, char *argv[])
{
	exit(EXIT_SUCCESS);
}
