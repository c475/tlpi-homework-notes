#include <signal.h>
#include "../../lib/tlpi_hdr.h"


typedef void (*signalhandler_t)(int);


// dummy handler for use with SIG_IGN
void dummy_handler(int sig)
{

}


void _sigset(int sig, signalhandler_t handler)
{
	if (handler == SIG_IGN) {
		if (signal(sig, dummy_handler) == -1) {
			return;
		}
	} else if (handler == SIG_DFL) {
		// this has to be cheating lol
		if (signal(sig, SIG_DFL) == -1) {
			return;
		}
	} else if (handler == SIG_HOLD) {
		
	} else {

	}


}


int _sighold(int sig)
{

}


int _sigrelse(int sig)
{

}


int _sigignore(int sig)
{

}


int _sigpause(int sig)
{

}