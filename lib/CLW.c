
#include "CLW.h"
#ifdef __APPLE__ 
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static cl_platform_id s_platformId = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int wclInitialize()
{
	if (clGetPlatformIDs(1, &s_platformId , 0) != CL_SUCCESS)
	{
		s_platformId = 0;
		return 0;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printOrAppendString(char* output, cl_device_id dev, cl_device_info param, int id, int index, const char* fmt, int* len)
{
	char tempBuffer[2048];

    size_t valueSize;
    char* value;

	clGetDeviceInfo(dev, param, 0, 0, &valueSize);
	value = (char*) malloc(valueSize);
	clGetDeviceInfo(dev, param, valueSize, value, 0);

	if (output && (*len > 0))
	{
		size_t tempLen;

		sprintf(tempBuffer, fmt, id, index, value);
		tempLen = strlen(tempBuffer);

		if (tempLen < *len)
		{
			strcat(output, tempBuffer);
			*len -= tempLen;
		}
	}
	else
	{
		printf(fmt, id, index, value);

	}

	free(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printOrAppendInt(char* output, cl_device_id dev, cl_device_info param, int id, int index, const char* fmt, int* len)
{
	char tempBuffer[2048];
    size_t value;

	clGetDeviceInfo(dev, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(value), &value, 0);

	if (output && (*len > 0))
	{
		int tempLen;

		sprintf(tempBuffer, fmt, id, index, value);
		tempLen = strlen(tempBuffer);

		if (tempLen < *len)
		{
			strcat(output, tempBuffer);
			*len -= tempLen;
		}
	}
	else
	{
		printf(fmt, id, index, value);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int wclListDevices(char* output, int size)
{
    int i, j;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;

    if (s_platformId == 0)
    	return 0;

    // get all platforms
    clGetPlatformIDs(0, 0, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, 0);

    for (i = 0; i < platformCount; ++i) 
    {
        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, 0, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, 0);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; ++j) 
        {
			printOrAppendString(output, devices[j], CL_DEVICE_NAME, j + 1, 0, "%d.%d Device: %s\n", &size);
			printOrAppendString(output, devices[j], CL_DEVICE_VERSION, j + 1, 1, " %d.%d Hardware version: %s\n", &size);
			printOrAppendString(output, devices[j], CL_DEVICE_VERSION, j + 1, 2, " %d.%d Software version: %s\n", &size);
			printOrAppendString(output, devices[j], CL_DEVICE_OPENCL_C_VERSION, j + 1, 3, " %d.%d OpenCL C version: %s\n", &size);
			printOrAppendInt(output, devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, j + 1, 4, " %d.%d Parallel compute units: %d\n", &size);
        }

        free(devices);
    }

    free(platforms);

    return size;
}



