#ifndef GET_NUM_HEADER

#define GET_NUM_HEADER

/*
	Flags affecting the behavior of getLong() and getInt()
*/

#define GN_NONNEG	01 /* Value must be >= 0 */
#define GN_GT_0		02 /* Value must be > 0 */

#define GN_ANY_BASE 0100 // can use any number base
#define GN_BASE_8	0200 // octal
#define GN_BASE_16	0400 // hex


long getLong(const char *arg, int flags, const char *name);

int getInt(const char *arg, int flags, const char *name);

#endif
