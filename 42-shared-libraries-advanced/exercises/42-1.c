#define _GNU_SOURCE
#include <dlfcn.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    void *lib1;
    void *lib2;
    void (*funcp)(void); // pointer to function with no arguments

    if (argc != 4) {
        usageErr("%s lib1 lib2 func (lib2 should have a symbol dependency on lib1)\n", argv[0]);
    }

    lib1 = dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL);
    if (lib1 == NULL) {
        fatal("Could not load library %s\n", argv[1]);
    }

    lib2 = dlopen(argv[2], RTLD_NOW);
    if (lib2 == NULL) {
        fatal("Could not load library %s\n", argv[2]);
    }

    // "unload" the library that has a symbol lib2 depends on
    dlclose(lib1);

    // load the function from lib2 that has a symbol dependency on lib1
    *(void **)(&funcp) = dlsym(RTLD_NEXT, argv[3]);
    if (funcp == NULL) {
        errExit("dlsym");
    }

    (*funcp)();

    exit(EXIT_SUCCESS);
}
