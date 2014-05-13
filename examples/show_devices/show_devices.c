#include <CLW.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This example will list the availible OpenCL devices in your system
// 

int main(int argc, const char* argv[])
{
	(void)argc;
	(void)argv;

	if (!wclInitialize())
		return -1;

	wclListDevices(0, 0);
}
