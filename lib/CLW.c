
#include "CLW.h"
#ifdef __APPLE__ 
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static cl_platform_id s_platformId = 0;
static struct OCLWDevice** s_devices = 0;
static int s_deviceCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct OCLWDevice
{
	cl_device_id deviceId;
	cl_context context;
} OCLWDevice;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* mallocZero(size_t size)
{
	void* t = malloc(size);
	assert(t);
	memset(t, 0, size);
	return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printOrAppendString(char* output, cl_device_id dev, cl_device_info param, int id, int index, const char* fmt, size_t* len)
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

static void printOrAppendInt(char* output, cl_device_id dev, cl_device_info param, int id, int index, const char* fmt, size_t* len)
{
	char tempBuffer[2048];
    size_t value;

	clGetDeviceInfo(dev, param, sizeof(value), &value, 0);

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OCLWDevice** wclGetAllDevices(int* count)
{
    cl_uint i, j, deviceIter = 0;
    cl_uint platformCount;
    cl_uint deviceCount;
    cl_platform_id* platforms;
    cl_device_id* devices;
	int totalDeviceCount = 0;

    if (s_platformId == 0)
    	return 0;

    // Check if we have already fetched all devices then we just return them here

    if (s_deviceCount > 0)
    {
    	*count = s_deviceCount; 
    	return s_devices; 
    }

    clGetPlatformIDs(0, 0, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, 0);

    for (i = 0; i < platformCount; ++i) 
    {
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, 0, &deviceCount);
		totalDeviceCount += deviceCount;
	}

	// Allocate space for the devices and then set them up

	s_devices = malloc((sizeof(OCLWDevice*) * (size_t)totalDeviceCount));

	// setup the device ids

    for (i = 0; i < platformCount; ++i) 
    {
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, 0, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, 0);

        for (j = 0; j < deviceCount; ++j, ++deviceIter) 
        {
			s_devices[deviceIter] = mallocZero(sizeof(OCLWDevice));
			s_devices[deviceIter]->deviceId = devices[j];
        }

        free(devices);
    }

    free(platforms);

    *count = totalDeviceCount;
	s_deviceCount = totalDeviceCount;

    return s_devices;
}

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

int wclListDevices(char* output, size_t size)
{
	int i, count;
	OCLWDevice** devices;

    if (s_platformId == 0)
    	return 0;

	devices = wclGetAllDevices(&count);

	for (i = 0; i < count; ++i)
	{
		cl_device_id id = 0;

		if (!devices[i])
			continue;

		id = devices[i]->deviceId;

		printOrAppendString(output, id, CL_DEVICE_NAME, i + 1, 0, "%d.%d Device: %s\n", &size);
		printOrAppendString(output, id, CL_DEVICE_VERSION, i + 1, 1, " %d.%d Hardware version: %s\n", &size);
		printOrAppendString(output, id, CL_DEVICE_VERSION, i + 1, 2, " %d.%d Software version: %s\n", &size);
		printOrAppendString(output, id, CL_DEVICE_OPENCL_C_VERSION, i + 1, 3, " %d.%d OpenCL C version: %s\n", &size);
		printOrAppendInt(output, id, CL_DEVICE_MAX_COMPUTE_UNITS, i + 1, 4, " %d.%d Parallel compute units: %d\n", &size);
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
struct OCLWDevice** wclGetBestDevices(int* count)
{


}
*/


