#define _GNU_SOURCE
#include <dlfcn.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    void *libHandle; // handle for shared library
    void (*funcp)(void); // pointer to function with no arguments
    const char *err;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s lib-path func-name\n", argv[0]);
    }

    // load the shared library and get a handle for it
    libHandle = dlopen(argv[1], RTLD_LAZY);
    if (libHandle == NULL) {
        fatal("dlopen: %s", dlerror());
    }

    // search library for symbol in argv[2]

    (void)dlerror(); // clear dlerror
    *(void **)(&funcp) = dlsym(libHandle, argv[2]);
    err = dlerror();
    if (err != NULL) {
        fatal("dlsym: %s", err);
    }

    // if the address from dlsym() is not NULL,
    // try calling it as a function that takes no arguments
    if (funcp == NULL) {
        printf("%s is NULL\n", argv[2]);
    } else {
        (*funcp)();
    }

    dlclose(libHandle); // close the library

    exit(EXIT_SUCCESS);
}
