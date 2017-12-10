#include <stdlib.h>
#include "../../lib/tlpi_hdr.h"


// the goal is to see at what size malloc() will employ mmap() instead of brk() to allocate memory
// the answer is:
// strace ./malloc_strace 134537
// 134537 bytes
// though it's more complicated than that

/*
    brk(NULL)                               = 0x564f21ecf000
    brk(0x564f21ef0000)                     = 0x564f21ef0000
    mmap(NULL, 135168, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f73be309000
*/

// so, it allocates more than enough memory with brk() (though space for bookkeeping is needed too)
// then also creates a memory mapping of the same size
// not sure why
int main(int argc, char *argv[])
{
    int i;
    char *x;

    i = getInt(argv[1], 0, "i");
    x = malloc(i);

    free(x);
    return 0;
}
