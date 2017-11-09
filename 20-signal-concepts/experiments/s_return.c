#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


// Saved states of CPU registers to unwind the stack

// struct sigcontext_64 {
// 	__u64				r8;
// 	__u64				r9;
// 	__u64				r10;
// 	__u64				r11;
// 	__u64				r12;
// 	__u64				r13;
// 	__u64				r14;
// 	__u64				r15;
// 	__u64				di;
// 	__u64				si;
// 	__u64				bp;
// 	__u64				bx;
// 	__u64				dx;
// 	__u64				ax;
// 	__u64				cx;
// 	__u64				sp;
// 	__u64				ip;
// 	__u64				flags;
// 	__u16				cs;
// 	__u16				gs;
// 	__u16				fs;
// 	__u16				ss;
// 	__u64				err;
// 	__u64				trapno;
// 	__u64				oldmask;
// 	__u64				cr2;

// 	/*
// 	 * fpstate is really (struct _fpstate *) or (struct _xstate *)
// 	 * depending on the FP_XSTATE_MAGIC1 encoded in the SW reserved
// 	 * bytes of (struct _fpstate) and FP_XSTATE_MAGIC2 present at the end
// 	 * of extended memory layout. See comments at the definition of
// 	 * (struct _fpx_sw_bytes)
// 	 */
// 	__u64				fpstate; /* Zero when no FPU/extended context */
// 	__u64				reserved1[8];
// };

int main(void)
{
	struct sigcontext ctx;
	// let's see what happens when a bad sigcontext is used...
	// warning: sigreturn is not implemented and will always fail
	// oh well
	memset(&ctx, 0, sizeof(struct sigcontext));
	sigreturn(&ctx);
	exit(EXIT_SUCCESS);
}
