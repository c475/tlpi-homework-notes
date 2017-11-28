#include <stdlib.h>
#include "../../lib/tlpi_hdr.h"


int main(int argc, char *argv[])
{
    system("cat /proc/sysvipc/shm");
    exit(EXIT_SUCCESS);
}
