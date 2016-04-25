#ifndef ERROR_HEADER

#define ERROR_HEADER

void errMsg(const char *format, ...);

#ifdef __GNUC__

    /* 
        This macro stops 'gcc -Wall' complaining that "control reaches
        end of non-void function" if we use the following functions to
        terminate main() or some other non-void function.
    */

    #define NORETURN __attribute__ ((__noreturn__))

#else

    #define NORETURN

#endif


/*
    The error function prototypes
*/
void errExit(const char *format, ...) NORETURN;

/*
    How it is different from errExit():

    It doesn’t flush standard output before printing the error message.
    It terminates the process by calling _exit() instead of exit(). This causes the pro-
    cess to terminate without flushing stdio buffers or invoking exit handlers.

    For now, we simply note that err_exit() is especially useful if we write a library function
    that creates a child process that needs to terminate because of an error.

    This termination should occur without flushing the child’s copy of the parent’s (i.e., the call-
    ing process’s) stdio buffers and without invoking exit handlers established by the
    parent.
*/
void err_exit(const char *format, ...) NORETURN;

/*
    errno = pthread_create(&thread, NULL, func, &arg);
    if (errno != 0) errExit("pthread_create");

    However, this approach is inefficient because errno is defined in threaded pro-
    grams as a macro that expands into a function call that returns a modifiable lvalue.
    Thus, each use of errno results in a function call. The errExitEN() function allows us
    to write a more efficient equivalent of the above code:

    int s;
    s = pthread_create(&thread, NULL, func, &arg);
    if (s != 0) errExitEN(s, "pthread_create");
*/
void errExitEN(int errnum, const char *format, ...) NORETURN;

/*
    The fatal() function is used to diagnose general errors, including errors from
    library functions that don’t set errno. Its argument list is the same as for printf(),
    except that a terminating newline character is automatically appended to the out-
    put string. It prints the formatted output on standard error and then terminates
    the program as with errExit().
*/
void fatal(const char *format, ...) NORETURN;


/*
    For usage error helper in [command line]? programs, like passing -h
*/
void usageErr(const char *format, ...) NORETURN;
void cmdLineErr(const char *format, ...) NORETURN;

#endif
