#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
	printf("Hello world\n");
	write(STDOUT_FILENO, "Ciao\n", 5);

	if (fork() == -1) {
		errExit("fork");
	}

	// both child and parent continue execution here
	exit(EXIT_SUCCESS);
}


https://play.pokemonshowdown.com/crossdomain.php?host=play.pokemonshowdown.com&path=battle-randombattle-426681727