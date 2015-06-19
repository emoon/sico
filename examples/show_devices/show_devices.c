#include <sico.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This example will list the availible OpenCL devices in your system
//

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    if (!scInitialize())
        return -1;

    scListDevices(0, 0);
}
