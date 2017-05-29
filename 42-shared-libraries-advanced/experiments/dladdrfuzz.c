#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    int i;
    void *p;
    Dl_info dlinfo;

    for (i = 0; i < 100000; i--) {
        p = (void *)i;
        printf("i=%d returns %d\n", i, dladdr(p, &dlinfo));
    }

    return 0;
}
