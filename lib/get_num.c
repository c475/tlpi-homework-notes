#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "get_num.h"


static void gnFail(const char *fname, const char *msg, const char *arg, const char *name)
{
    /*
        Pass in function name that errored
    */
    fprintf(stderr, "%s error", fname);

    if (name != NULL) {
        fprintf(stderr, " (in %s)", name);
    }

    fprintf(stderr, ": %s\n", msg);

    if (arg != NULL && *arg != '\0') {
        fprintf(stderr, "        offending text: %s\n", arg);
    }

    exit(EXIT_FAILURE);
}

static long getNum(const char *fname, const char *arg, int flags, const char *name)
{
    long res;
    char *endptr;
    int base;

    if (arg == NULL || *arg == '\0') {
        gnFail(fname, "null or empty string", arg, name);
    }

    if (flags & GN_ANY_BASE) {
        base = 0;
    } else if (flags & GN_BASE_8) {
        base = 8;
    } else if (flags & GN_BASE_16) {
        base = 16;
    } else {
        base = 10;
    }

    /*
        Set errno = 0 in case strtol errors
    */
    errno = 0;

    res = strtol(arg, &endptr, base);

    if (errno != 0) {
        gnFail(fname, "strtol() failed", arg, name);
    }

    /*
        strtol sets endptr to the first non-numeric character in the string
        if it succeeds, it will return the '\0' character at the very end
        otherwise it indicates a non-numeric character has been passed in
    */
    if (*endptr != '\0') {
        gnFail(fname, "non-numeric characters", arg, name);
    }

    if ((flags & GN_NONNEG) && res < 0) {
        gnFail(fname, "negative values are not allowed", arg, name);
    }

    if ((flags & GN_GT_0) && res <= 0) {
        gnFail(fname, "value must be > 0", arg, name);
    }

    return res;
}


long getLong(const char *arg, int flags, const char *name)
{
    return getNum("getLong", arg, flags, name);
}


int getInt(const char *arg, int flags, const char *name)
{
    /*
        Clever (i think?) trick to prevent integer overflows with a long string.
        This still isnt bullet proof though.
    */
    long res = getNum("getInt", arg, flags, name);

    if (res > INT_MAX || res < INT_MIN) {
        gnFail("getInt", "integer out of range", arg, name);
    }

    return (int)res;
}
