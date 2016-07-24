#include <signal.h>


typedef void (*sighandler_t)(int);


sighandler_t signal(int sig, sighandler_t handler)
{
	struct sigaction newDisp, prevDisp;

	newDisp.sa_handler = handler;

#ifdef OLD_SIGNAL
	newDisp.sa_flags = SA_RESETHAND | SA_NODEFER;
#else
	newDisp.as_flags = SA_RESTART
#endif

	if (sigaction(sig, &newDisp, &prevDisp) == -1) {
		return SIG_ERR;
	} else {
		return prevDisp.sa_handler;
	}
}
