#include <sys/prctl.h>
#include "caps_only.h"


// call on program startup preferably
// int caps_only(void)
// {
// 	return prctl(
// 		PR_SET_SECUREBITS,
// 		SECBIT_NO_SETUID_FIXUP | SECBIT_NO_SETUID_FIXUP_LOCKED |
// 		SECBIT_NOROOT | SECBIT_NOROOT_LOCKED
// 	);
// }
