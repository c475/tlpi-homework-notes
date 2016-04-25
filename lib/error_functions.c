/*
    va_list, va_start, etc
*/
#include <stdarg.h>

#include "error_functions.h"
#include "tlpi_hdr.h"

/*
    Defines ename and MAX_ENAME
*/
#include "ename.c.inc"

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif


static void terminate(Boolean useExit3)
{
    /* 
        Dump core if EF_DUMPCORE environment variable is defined and
        is a nonempty string; otherwise call exit(3) or _exit(2),
        depending on the value of 'useExit3'.
    */

    char *s = getenv("EF_DUMPCORE");

    if (s != NULL && *s != '\0') {
        abort();
    } else if (useExit3) {
        exit(EXIT_FAILURE);
    } else {
        _exit(EXIT_FAILURE);
    }
}


static void outputError(Boolean useErr, int err, Boolean flushStdout, const char *format, va_list ap)
{
    #define BUF_SIZE 500

    char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

    /*
        It's like snprintf except it works for va_args

        snprintf(userMsg, BUF_SIZE, "%s! I'm %s\n", "Hi!", "SomeDude");

        So start by buffering the custom error message passed in.
    */
    vsnprintf(userMsg, BUF_SIZE, format, ap);

    /*
        Decide to use the error names defined in ename.c.inc or not
    */
    if (useErr) {
        snprintf(
            errText,
            BUF_SIZE,
            " [%s %s]",
            (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN?", strerror(err)
        );
    } else {
        snprintf(errText, BUF_SIZE, ":");
    }

    /*
        Then buffer the resulting error string
    */
    snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

    if (flushStdout) {
        fflush(stdout);
    }

    fputs(buf, stderr);

    /*
        In case stderr is not line-buffered
    */
    fflush(stderr);
}


void errMsg(const char *format, ...)
{
    va_list argList;
    int savedErrno;

    /*
        In case we change it here
    */
    savedErrno = errno;

    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);

    errno = savedErrno;
}


void errExit(const char *format, ...)
{
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}


void err_exit(const char *format, ...)
{
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errno, FALSE, format, argList);
    va_end(argList);

    terminate(FALSE);
}


void errExitEN(int errnum, const char *format, ...)
{
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errnum, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}


void fatal(const char *format, ...)
{
    va_list argList;

    va_start(argList, format);
    outputError(FALSE, 0, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}


void usageErr(const char *format, ...)
{
    va_list argList;

    fflush(stdout);

    fprintf(stderr, "Usage: ");

    va_start(argList, format);

    /*
        Like vsnprintf with va_args but you write to a file descriptor
    */
    vfprintf(stderr, format, argList);

    va_end(argList);

    fflush(stderr);

    exit(EXIT_FAILURE);
}


void cmdLineErr(const char *format, ...)
{
    va_list argList;

    fflush(stdout);

    fprintf(stderr, "Command-line usage error: ");

    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);

    exit(EXIT_FAILURE);
}
